param(
    [Parameter(Mandatory = $true)]
    [string]$Executable,

    [Parameter(Mandatory = $true)]
    [string]$Output
)

Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;
using System.Runtime.InteropServices;

public static class WindowCaptureNative
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Rect
    {
        public int Left;
        public int Top;
        public int Right;
        public int Bottom;
    }

    [DllImport("user32.dll")]
    public static extern bool GetWindowRect(IntPtr window, out Rect rect);

    [DllImport("user32.dll")]
    public static extern bool SetForegroundWindow(IntPtr window);

    [DllImport("dwmapi.dll")]
    public static extern int DwmFlush();
}
"@

$process = Start-Process -FilePath $Executable -PassThru

try
{
    if (-not $process.WaitForInputIdle(15000))
    {
        throw "Application did not become input-idle."
    }

    $process.Refresh()
    $window = $process.MainWindowHandle
    if ($window -eq [IntPtr]::Zero)
    {
        throw "Application did not expose a main window."
    }

    [WindowCaptureNative]::SetForegroundWindow($window) | Out-Null
    [WindowCaptureNative]::DwmFlush() | Out-Null

    $rect = New-Object WindowCaptureNative+Rect
    if (-not [WindowCaptureNative]::GetWindowRect($window, [ref]$rect))
    {
        throw "Failed to read the application window bounds."
    }

    $width = $rect.Right - $rect.Left
    $height = $rect.Bottom - $rect.Top
    $bitmap = New-Object System.Drawing.Bitmap($width, $height)
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)

    try
    {
        $graphics.CopyFromScreen($rect.Left, $rect.Top, 0, 0, $bitmap.Size)
        $bitmap.Save($Output, [System.Drawing.Imaging.ImageFormat]::Png)
    }
    finally
    {
        $graphics.Dispose()
        $bitmap.Dispose()
    }

    [PSCustomObject]@{
        ProcessId = $process.Id
        Responding = $process.Responding
        Width = $width
        Height = $height
        Screenshot = $Output
    } | ConvertTo-Json -Compress
}
finally
{
    if (-not $process.HasExited)
    {
        Stop-Process -Id $process.Id -Force
    }
}

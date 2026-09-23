# 004.Camera FPS camera

`003.BoxApp` remains the fixed-camera cube sample, including ImGui cube rotation and color controls. `004.Camera` is a separate executable with the same cube controls plus the engine FPS controller. Select `004.Camera` as the startup project to use FPS navigation.

Hold the right mouse button **outside the ImGui panel** to control the camera:

| Input | Action |
| --- | --- |
| Mouse | Look left/right and up/down |
| W / S | Move forward/backward on the horizontal plane |
| A / D | Strafe left/right |
| Q / E | Move down/up |
| Shift | Move three times faster |
| Release right mouse / Escape | End camera control and restore the cursor |

The camera releases capture when the window loses focus, is minimized, begins moving/resizing, or closes. A right-click that starts on ImGui cannot activate the camera by dragging outside the panel. Text and keyboard input owned by ImGui blocks camera movement.

## Ownership

- `DirectX12Sample/FpsCameraController.cpp`: position, yaw/pitch, normalized movement and view updates. `Update(Camera&, Input, seconds)` accepts device-independent frame input.
- `DirectX12Sample/FpsCameraControllerWin32.cpp`: Win32 keyboard/mouse polling, right-button activation, cursor ownership and focus/capture cleanup.
- `ImGuiLayer` and `GraphicsCore`: expose `UIInputCapture` without leaking ImGui backend details.
- `004.Camera/MainApp`: chooses the initial pose and settings, forwards window messages, calls the engine controller before submitting the camera view.

`Camera` remains responsible for view/projection matrices. Existing clients do not opt into FPS control and keep their previous behavior. Mouse sensitivity is radians per pixel; movement speed is world units per second. Pitch is limited below 90 degrees. Movement time is capped at 0.1 seconds to avoid jumps after a stalled frame. Cube geometry, rotation and color do not change when the camera moves.

`tests/FpsCameraContract.cpp` verifies movement, frame-time behavior, diagonal normalization, boosting, yaw-relative movement, pitch limits and preservation of projection.

`tests/FpsCameraInputContract.cpp` compiles the production Win32 adapter with deterministic OS boundary replacements. It checks input ownership, mouse recentering and capture cleanup without changing the user's keyboard or cursor state; this does not replace a manual held-button check on the target machine.

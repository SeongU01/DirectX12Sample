# DirectX12Sample Client Tools Design System

## 0. Research Log

- Embedded reference: Dear ImGui 1.91.9 default dark style was selected because this is an operational client/debug surface, not a branded product UI.
- Lazyweb: skipped because browser-product layout references do not apply to a native Win32 immediate-mode tool.
- Imagen drafts: skipped because the user explicitly requested a functional ImGui control using the library's existing visual language.

## 1. Atmosphere & Identity

The client tools are a compact, predictable engineering control surface. The signature is immediate feedback: changing a value updates the rendered DirectX frame in the same frame without decorative motion or hidden state.

## 2. Color

### Palette

The tool surface uses `ImGui::StyleColorsDark()` without local overrides. Widget colors, hover colors, focus colors, borders, and text colors therefore come from the Dear ImGui style table. The Back Buffer color is user-controlled scene data and is not a UI theme token.

### Rules

- Do not add local hard-coded ImGui style colors.
- Keep one UI theme across all client tool panels.
- Use color only for widget state and the user-selected Back Buffer value.

## 3. Typography

- Primary: Dear ImGui embedded default font.
- Body and labels: Dear ImGui default size and metrics.
- Labels remain concise English until a Korean-capable font atlas is deliberately added.
- Do not introduce a second font family for this milestone.

## 4. Spacing & Layout

- Base unit: Dear ImGui default style spacing.
- Window padding, frame padding, item spacing, and indentation come from the active ImGui style.
- The control panel remains movable and collapsible, and automatically follows its content size as controls are added.
- This milestone keeps the existing 1600 x 900 host-window behavior; a native resize matrix is deferred.

## 5. Components

### Initialize Controls Panel

- **Structure**: one `ImGui::Begin()` window containing a short description and a `ColorEdit4` field.
- **Variants**: open or collapsed using Dear ImGui's standard window affordance.
- **Spacing**: inherited from the Dear ImGui style.
- **States**: default, hovered, keyboard-focused, actively edited, and collapsed.
- **Accessibility**: keyboard navigation enabled; visible focus follows Dear ImGui behavior; the control has a persistent text label.
- **Motion**: none beyond Dear ImGui's immediate interaction feedback.
- **Layout**: floating native debug panel rendered over the Back Buffer.

### Back Buffer Color Field

- **Structure**: `ColorEdit4` backed by four normalized linear RGBA floats.
- **Variants**: compact preview and the standard popup color editor supplied by Dear ImGui.
- **States**: default, hovered, focused, popup-open, dragging, numeric edit.
- **Accessibility**: keyboard navigation and numeric entry remain available.
- **Motion**: no decorative animation; the frame clear changes immediately.

## 6. Motion & Interaction

- State changes are immediate and occur in the current frame.
- No autonomous, looping, entrance, or decorative animation is allowed.
- Mouse and keyboard interaction use the standard Dear ImGui Win32 backend behavior.

## 7. Depth & Surface

- Strategy: Dear ImGui default borders and tonal surface hierarchy.
- The tool panel renders as a real ImGui window over the live DirectX Back Buffer.
- Do not fake the panel with a bitmap, static overlay, or pre-rendered image.

## 8. Accessibility Constraints & Accepted Debt

### Constraints

- Keyboard navigation is enabled through `ImGuiConfigFlags_NavEnableKeyboard`.
- The color control always has a visible text label and supports numeric entry.
- Focus and active states must remain visible under the default ImGui theme.
- The panel must not block closing the native application window.

### Accepted Debt

| Item | Location | Why accepted | Owner / Exit |
|------|----------|--------------|--------------|
| No screen-reader semantics | Dear ImGui native panel | Dear ImGui does not expose a standard Win32 accessibility tree for immediate-mode widgets | Revisit when the client tool grows beyond engineering-only use |
| English-only labels | ClientUI | The embedded default font does not guarantee Korean glyph coverage | Add a Korean font atlas before Korean UI copy is introduced |
| Fixed native-window QA size | 001.Initialize | This milestone targets the existing 1600 x 900 Win32 client | Add resize-matrix QA when resize handling is implemented |

## 9. Ownership

- The engine `ImGuiLayer` owns the ImGui context, Win32/DX12 backends, descriptor callbacks, frame lifecycle, rendering, message forwarding, and shutdown.
- Client code owns panel composition and application state, and may call the public `imgui.h` widget API only between the engine's UI frame boundaries.

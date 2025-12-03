# minecraft.cpp

## Troubleshooting
### Mouse input stops working while holding keyboard keys (Linux)
If users report that they cannot move the mouse cursor while holding down a non-modifier key (e.g., 'W' for movement) on Linux systems, this is almost certainly due to an operating system-level feature called "Disable While Typing" (DWT) or "Palm Detection," not an issue within the GLFW application code itself.  

Users on Linux systems using the Xorg display server can resolve this issue by disabling this OS feature via the terminal:

1.  **Identify the touchpad device:**
    ```bash
    xinput list
    ```
    (Note the ID of the touchpad device, e.g., `17`)

2.  **Disable the "Disable While Typing" feature:**
    ```bash
    xinput set-prop <device_id> "libinput Disable While Typing Enabled" 0
    ```
    *(Replace `<device_id>` with the actual ID, e.g., `xinput set-prop 17 "libinput Disable While Typing Enabled" 0`)*

**Note:** This change is usually temporary and reverts upon system reboot or logging out. Users must configure their operating system's startup applications to run this command automatically if they want a permanent fix.



## BrowserWindowStyles

This demo program is intended to explore possible solutions for the following electron feature request:
https://github.com/electron/electron/issues/6823

The end goal is to eliminate the wasted space in the standard Windows titlebar. The proposed solution would make additional options available when creating the BrowserWindow electron object which allow the user to have a frameless window with the minimize, maximize, and close buttons. They should be toggle the menu (not currently available with frameless), which would also share this same non-client area. This feature would then be used to solve the following issue:
https://github.com/brave/browser-laptop/issues/3036

### Giving credit where it's due
In the code, I've tried to attribute the sources where applicable. Sources that were invaluable:
- MSDN
- Stack Overflow
- http://www.catch22.net/tuts/custom-titlebar


# roblox_device_spoof
lets you join roblox console only games

you could make it spoof as Mobile/Tablet device but, that string is much longer and may need a different apporoach

# usage
**Method 1:**

rename RobloxPlayerBeta.exe to ogRobloxPlayerBeta.exe

rename this executable to RobloxPlayerBeta.exe and place it next to ogRobloxPlayerBeta.exe

**Method 2 (recommended):**

replace the original RobloxPlayerLauncher.exe in the roblox folder with the one from here

this will also prevent roblox from updating normally

# misc

if the scan isnt basicaly instant, then roblox didnt finish doing its thing yet, changing the Sleep time can help fix this

# joining game
cause roblox website removes the play button to play open devtools, edit the code snippet and run it in console
```js
Roblox.GameLauncher.joinMultiplayerGame("place id")
```
other ways of joining should work normally
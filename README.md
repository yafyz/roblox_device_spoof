# roblox_device_spoof
lets you join roblox console only games

you could make it spoof as Mobile/Tablet device but, that string is much longer and may need a different apporoach

# usage
rename RobloxPlayerBeta.exe to ogRobloxPlayerBeta.exe

rename this executable to RobloxPlayerBeta.exe and place it next to ogRobloxPlayerBeta.exe

if the scan isnt basicaly instant, then roblox didnt finish doing its thing yet, changing the Sleep time can help fix this

# joining game
cause roblox website removes the play button to play open devtools, edit the code snippet and run it in console
```js
Roblox.GameLauncher.joinMultiplayerGame("place id")
```
other ways of joining should work normally
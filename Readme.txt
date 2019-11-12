This project is nowhere near complete.

By commenting and uncommenting code, you can choose between
a few different configurations:

1. Enable 4 screens, control all 4 players with one controller
2. Enable 4 screens, control all 4 players independently, with major input lag
3. Enable 4 screens, control the first 2 players flawlessly while the other 2 remain idle

There must be a way to perfect this mod. Currently, it is possible to copy data
from P1's controller data buffer into P2, without touching memory in the 
PCSX2 controller plugin, so it must be possible to write to P3's and P4's buffer.

However, writing to P3 and P4 will trigger a detection, and a message will popup
saying controllers are disconnected. If we can get around that, then 4 people will
be able to play this game in PCSX2, split-screen, withotu LAN or Online
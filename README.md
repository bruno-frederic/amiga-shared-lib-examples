# Using *AmigaOS* shared libraries with VBCC examples

For french explanations, look at the comments of the .c files and *Makefile*.

For an english explanation, you can read these excellent articles :
- [*Start with C programming on Amiga*](https://medium.com/@8bitsten/start-with-c-programming-on-amiga-1e8312cec2db) by 8bitsten
- [*Amiga 500: How Libraries Work and How to Use Them*](https://bumbershootsoft.wordpress.com/2022/06/12/amiga-500-how-libraries-work-and-how-to-use-them/) by Michael Martin

## Building environment
I have used :
- Visual Studio Code 1.70 on Windows 10
- [AmigaOS 3.2 NDK Release 4](https://www.hyperion-entertainment.com/index.php/downloads) header files

Additionnaly, I have used these excellent packages by *prb28* to intregrate tightly VBCC into VS Code :
- [Amiga Assembly extension](https://marketplace.visualstudio.com/items?itemName=prb28.amiga-assembly) for VSCode
- VBCC binaries (+ GNU Make + custom FS/WinUAE for debugging) : [vscode-amiga-assembly-binaries](https://github.com/prb28/vscode-amiga-assembly-binaries)
- VS Code [example workspace for VBCC](https://github.com/bruno-frederic/vscode-amiga-vbcc-example/)


But it should compile easily on another environment with :
- [VBCC v0.9](http://sun.hasenbraten.de/vbcc/)
- AmigaOS 3.2 NDK
- Make


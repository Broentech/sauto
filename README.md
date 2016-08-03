# SAUTO

SAUTO is an worktask scheduling library implemented using Qt5

by Stian Broen, 2016
(C) Broentech Solutions AS
stian@broentech.no

## Installation

How do I build it?
	
First you need (for Windows) :

1. Qt 5.X
3. (Optional) Visual Studio
4. Git

Open a shell according to your setup. I use Qt 5.7 64-bit for Desktop (MSVC 2013)

Go to /scripts
run 
setEnv.cmd

go to /source
run
qmake -r
make

(if you have Visual Studio) :
run
qmake -r -tp vc
nmake

You can open the project in Visual Studio now
go to /scripts
run
vcstart.cmd
browse to your sln file and open it

## Usage

For details, see   
https://broentech.no/#!/nativeapps/57a1f047bd1dc493c687a68d


## License

LGPL v3, see separate file


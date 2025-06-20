![Oinut Logo](https://github.com/purplehuman/oinut/blob/master/resource/oinut-logo.png?raw=true)

Free and open source (GPLv3) animation software for Linux.

Homepage: [Oinut](http://oinut.org/)

What's with the name? Oinut is pronounced /ɔːjnɑːt/. It's pronounced the same as the word "oynat" in Turkish which means "play, make move". According to Google Translate it also means "placed great value" in Finnish as it is. 

Oinut is a raster based animation package, and can be used for traditional animation (either paper drawn or digital), cutout animation, stopmotion animation, rotoscoping, or combining them all.

The most important reason I (O. Bahri Gordebak) started developing Oinut is that I couldn't find a program to make cutout animation easily on Linux. So it started as a cutout animation tool. Then it evolved into something more general to do animation.

There's not much documentation at the moment, but we hope soon there will be. But the program is very functional.

If you have ffmpeg in your path, import video and render video buttons are enabled. But without ffmpeg, you can always render an image sequence, so it shouldn't be much of a problem.

To compile it from source, you'll need Qt5, opencv-core, opencv-highgui, zlib, sane and quazip-qt5 development packages. To run the software, you'll need binary packages of these libraries and Qt5 multimedia plugins.

Required Qt5 modules are: Qt5Svg, Qt5Widgets, Qt5Multimedia, Qt5Gui, and Qt5Core.

Documentation will go in the [wiki](https://github.com/purplehuman/oinut/wiki). Please see [Contributing page](https://github.com/purplehuman/oinut/wiki/Contributing) on the wiki or CONTRIBUTING file in the source for ways you can contribute. If you have an accepted code contribution, please don't forget to add yourself to the AUTHORS file.

[Here](https://github.com/purplehuman/oinut/wiki/Features) is a list of features.

See [Screenshots page](https://github.com/purplehuman/oinut/wiki/Screenshots) for screenshots.

Before making a feature request, please read the [TODO](https://github.com/purplehuman/oinut/wiki/TODO) to make sure it's not planned already.

Issue tracker can be found [here](https://github.com/purplehuman/oinut/issues)

Enjoy!

Note:

Making or importing long animations with high resolutions might cause high memory and cpu usage. We suggest working on pretty short animations or scenes at once if you're using high resolutions.

Installation
============

To compile Oinut from source, you'll need Qt5 (core, gui, widgets, multimedia and svg), opencv-core, opencv-highgui, zlib, sane and quazip-qt5 development packages. To run the software, you'll need binary packages of these libraries and Qt5 multimedia plugins.

Download the source to some location on your computer and unzip or untar it. Enter the extracted directory and run the commands below:

     qmake
     make
     make install (as root)

You can run the software with the following command:

     oinut

There are two mimetype files in resource folder named `x-oin.xml` and `x-oic.xml`. If you'd like to associate .oin files with Oinut use these commands:

     xdg-mime install x-oin.xml
     xdg-mime default oinut.desktop application/x-oin
     xdg-mime install x-oic.xml

Note: .oic files are Oinut cutout animation files and can't be associated right now.

Note 2: If you install from Ubuntu packages, file associations aren't made. We might add a postinstall script in the future. If you'd like file associations, just use `x-oin.xml` and `x-oic.xml` from the source as described above.



#!/bin/sh

for Dir in `find . -maxdepth 1 -type d`
do
    if [ -d $Dir/Plugin ]
        then
        name="${Dir:2}"

	echo "Building $name..."
	cd $Dir/Plugin
	$PROJUCER --resave $name.jucer
	cd Builds/MacOSX
	xcodebuild -project $name.xcodeproj/ clean
	xcodebuild -project $name.xcodeproj/ -configuration Release | xcpretty -s -f `xcpretty-travis-formatter`

        echo "Copying $name ..."
	cd ../../../../
        mkdir Bin/$name/Mac
        cp -R "$Dir/Plugin/Builds/MacOSX/build/Release/$name.app" Bin/$name/Mac # standalone
        cp -R "$Dir/Plugin/Builds/MacOSX/build/Release/$name.vst" Bin/$name/Mac # VST
        cp -R "$Dir/Plugin/Builds/MacOSX/build/Release/$name.vst3" Bin/$name/Mac # VST3
        cp -R "$Dir/Plugin/Builds/MacOSX/build/Release/$name.component" Bin/$name/Mac # AU

        zip -r Bin/$name/MacBuilds.zip Bin/$name/Mac
    fi
done

# HourglassGranular
### A granular synthesiser with a quantised heart.

![hourglass_img](https://i.imgur.com/dTnnzjO.png)

Hourglass is a granular synthesis engine capable of manipulating audio samples to produce a wide range of unique and distinctive timbres ranging from harsh, transient percussion to rich, evolving textures. 
A minimal yet powerful interface enables users to explore sounds quickly and efficiently, offering a balance between ease of use and depth of control.
An internal metronome allows grains to be spawned rhythmically at regular intervals, a feature targeted towards live musicians in a performance context.
Platform agnostic saving and loading plus DAW automation support allows hourglass to fit in with any musician’s plugin arsenal.

## Installation
This application runs in both Standalone executable and VST Plugin format.

Downloadable binaries can be found in releases, or you can build it yourself for your target platform.

### VST:
Copy the .dll into your VSTPlugins folder.

Typically `C:/Program Files/VSTPlugins`

### Standalone:
Run the executable from anywhere on your machine.

## Build Instructions
Built with [JUCE 5.1.2](https://juce.com/)

Open the .projucer file in the Projucer, then click the _Save and Open in IDE_ button.

The application should build with no additional libraries required.

Change your build target within the Projucer and download the relevant IDE to build for additional platforms.

## Usage

Once you have hourglass up and running on your machine, you can load in a sample to granulise by clicking _"Load Sample"_
Locate a .WAV you'd like to use, and once opened you will see the audio waveform displayed in the window.

Click on any label to manually enter in values.

### Grain Controls

**Window Blending** - The leftmost slider allows blending between different window shapes.
-	0 = Hanning
-	25 = Sine
-	50 = Triangle
-	75 = Boxcar (square)
-	100 = Hanning

**"Ext" Checkbox** - Choose whether to use Hourglass' own internal clock or the clock in your host application.

**"BPM" Slider** - Sets Hourglass' internal metronome.

**"DIV" Slider** - Sets a division of Hourglass' internal metronome which grains will spawn at.
 - E.g. A division of 4 will spawn each quarter note.

**"Start"** - Sets the start point for grain spawning.
 - 0 - 50% represents a fixed relative point between the start and end of the sample to spawn grains from - good for sharp, transient sounds
 - 50 - 100% repesents a moving point that will scrobble the sample - good for smeared, evolving textures.
		
**"Size"** - Sets the size of the grains being spawned. Low values will create clicky transient grains that quickly die. High values will mean long grains that will blur together.

**"Pitch"** - Sets the output frequency of the grains as a whole.

**"Random Pitch"** - Sets individual grains to have a random variation in frequency.

**"Random Start"** - Randomly varies the start position.

**"Random Pan"** - Randomly varies grains between the stereo channels.

**"phaser q"** - Sets Q factor for phaser allpass bank.

**"phaser speed"** - Sets speed of phaser LFO.

**"phaser depth"** - Sets intensity of phaser LFO.

## Saving & Recalling Presets
Presets can be saved and loaded using the options menu in the top left, or through your plugin host state saving functionality.

Loaded sample file location paths are saved but the audio files are not. A message will display if hourglass is unable to locate a missing sound file.

## Audio & MIDI Settings
In standalone mode, audio and MIDI settings can be accessed via the options menu in the top left.

The plugin will automatically assume audio & MIDI settings from the host.

UI design inspired by [ValhallaDSP](https://valhalladsp.com/)

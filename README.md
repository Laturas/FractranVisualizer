# Katelyn's Fractran Visualizer
This is a tool built for visualizing fractran programs. Each color corresponds to a prime factor, and the width of the color at each step indicates how many of that factor are in the current term.

It's meant to help visualize the execution of this rather esoteric turing complete system.

## Compiling
To compile, just compile main.c with your C compiler of choice. There are no external dependencies.

If you're using gcc (what this was built with), you can run the compile.sh command.

## Usage
Run the executable from command line with the following flags:
- `-help` for a list of these flags
- `-width` and `-height` to specify image dimensions.
- `-s` To specify how many steps to run the fractran program.
- `-o` To specify an output file.
- A fractran program, specified as a comma separated list of fractions. All other styling is ignored (bad practice ik ik ik).

As an example of these in action, this is the command used to generate one of the example images:

`./fractran_visualizer.exe '[7/15, 22/3, 6/77, 5/2, 9/5]' -s 800 -width 256 -height 800 -o bbf20_champ.png`

## Contributing
Feel free to make a pull request! Though I'm busy and I can't guarantee I'll be able to review it immediately.

## Can I use this for X?
Yes! You may use it for whatever you want. Just be sure to credit :)

Also let me know because I love seeing how my stuff gets used!

## Are there bugs?
Probably. Some things may not be programmed ideally either. From my testing it produces correct results but I wouldn't rely on it for anything mathematically rigorous.

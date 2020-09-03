# Game Of Life

### This is an implementation of the famous Game of Life cellular automaton in C++ using the [olcConsoleGameEngine](https://github.com/howlettjames/videos/blob/master/olcConsoleGameEngineSDL.h). Check usage and results of this work ![here](https://github.com/howlettjames/Game-Of-Life/blob/master/Docs/LIFE_REPORT.pdf "Usage and results").

## Summary
This implementation provides several simulation options as:

* Choosing the rules by which the GOL will simulate, the default one is B3/S23

* The ability load a pre configuration file i.e. a file containing an already simulated state of the automata.

* The ability to choose the percentage of live cells at the beginning of the simulation.

* There are two types of Pattern Recognition inside the simulation to choose:
    * Oscilators-Still-Chaos: This will represent the Oscilating life forms, Still lifes and 'Chaos' (any other) in different colors.

    * Static-NonStatic: This will only show two different kind of cells, those who stay still over more than one generation and those who not.

* The ability to slow down the simulation by maintaining the function keys pressed:
    * F1 delays for 100ms.
    * F2 delays for 200ms.
    * F3 delays for 300ms.
    * F5 delays for 500ms.
    * F10 delays for 1s.
    * F11 delays for 2s.

* Finally, it is also posible to stop the simulation at any generation using the Esc key, then you will be asked if a configuration file with the actual generation should be saved or exit otherwise.

It also does a computation of several variables over each generation:

* Average cell life forms.
* Shannon Entropy calculation.
* Variance
* Number of Oscilators, Still or Chaos forms.

## Run the program

In order to run the program you need to:
* Download the code.
* Run in a Linux distribution (Ubuntu recommended).
* Having [SDL2](https://www.libsdl.org/download-2.0.php) and C++ installed on your computer.
* Compile with:
```bash
g++ thing.cpp -DUNICODE -I/usr/include/SDL2 -lSDL2 -lpthread -std=c++11 -o game.out
```
* Run:
```bash
./game.out
```

## Example of a simulation

![Running the simulation](https://github.com/howlettjames/Game-Of-Life/blob/master/Test/test.png "Running the simulation")
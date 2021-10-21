# romato

[![License - MIT](https://img.shields.io/badge/license-MIT-green)](https://spdx.org/licenses/MIT.html)

![romato](docs/images/pomodoro.svg "romato")
(rotten tomato? [Fresh Fruit for Rotting Vegetables](
https://en.wikipedia.org/wiki/Fresh_Fruit_for_Rotting_Vegetables)! :stuck_out_tongue_winking_eye:)

-----

romato is a library that is used to build some of my tools with current MSVC
compilers. It serves two main purposes:

 - Because I like small independent executables, I try to avoid the compiler
   runtime library. Therefore romato supplies some basic functionality (e.g.
   memcpy).
 - Several of my tools require similar (base) classes for strings, windows,
   dialogs etc. romato provides those classes.

-----

Note: Most likely romato is _**not**_ the right choice for you! It contains
various assumptions that are OK for my tools, but surely not for the general
case (e.g. it raises a fatal exception if a window could not be created).

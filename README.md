# Kaleidoscope: Implementing a Language with LLVM

## Why?

Self-education...

I'm interested in LLVM and want to try simple things with it.
That's why I've started official LLVM tutorial - [Kaleidoscope](http://llvm.org/docs/tutorial).

## What's all about?

This tutorial runs through the implementation of a simple language, showing how fun and easy it can be.
This tutorial will get you up and started as well as help to build a framework you can extend to other languages.
The code in this tutorial can also be used as a playground to hack on other LLVM specific things.

The goal of this tutorial is to progressively unveil our language, describing how it is built up over time.
This will let us cover a fairly broad range of language design and LLVM-specific usage issues, showing and explaining the code for it all along the way, without overwhelming you with tons of details up front.

It is useful to point out ahead of time that this tutorial is really about teaching compiler techniques and LLVM specifically, not about teaching modern and sane software engineering principles.
In practice, this means that we’ll take a number of shortcuts to simplify the exposition.
For example, the code uses global variables all over the place, doesn’t use nice design patterns like visitors, etc... but it is very simple.
If you dig in and use the code as a basis for future projects, fixing these deficiencies shouldn’t be hard.

## How it works all together?

TODO, when I'll know more :)

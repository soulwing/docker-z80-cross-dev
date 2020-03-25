z80-bin
=======

Converts a binary image file produced by `makebin` and adds the prefix
necessary to allow it to be used by `z80-mon`.

A typical workflow for assembling (non-relocatable) code and running in 
`z80-mon` is as follows.

```
zmac -o foobar.hex foobar.asm
makebin foobar.hex foobar.bin
z80-fix <foobar.bin >foobar.z80
z80-mon foobar.z80
```

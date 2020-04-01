docker-z80-cross-dev
====================

This repository contains a Dockerfile and supporting source code to make 
a Docker container image that can be used to support Z80 cross-development 
on any system that can run Docker.

The resulting container image has all of the following tools installed.

* [zmac](http://48k.ca) -- Z80 assembler
* [ld80](http://48k.ca) -- LD80 linker 
* [z80-asm](http://wwwhomes.uni-bielefeld.de/achim/z80-asm.html) -- 
  Z80 assembler and (excellent) Z80 emulator
* [sdcc](http://sdcc.sourceforge.net/) -- Small Device C Compiler
* [gcc](https://gcc.gnu.org/) -- GNU C compiler
* [make](https://www.gnu.org/software/make/) -- GNU make
* [binutils](https://www.gnu.org/software/binutils/) -- GNU binutils
* [vim](https://www.vim.org/) -- VIM editor

Licensing
---------

The container image specification in the `Dockerfile`, as well as the 
`z80-bin` and `z80-ports` utilities contained in this repository are 
licensed under the Apache License 2.0.

The tools installed in the container are covered by a variety of open-source
licenses. In your use of this container image, you are obliged to comply with
the license terms of the tools that you choose to use for your work.

Build the Image
---------------

```
docker image build -t soulwing/z80 .
```

Use the Image
-------------

Once the image has been built, you can use it at any time to do Z80 
cross-development. The recommended approach is to mount a directory from
your host system into the Docker container so that files that you edit,
assemble, link, etc are available on the host after the container has
exited.

```
docker container run --rm -v $PWD:/Z80 -ti soulwing/z80 
```

After the container is running, you can also use `docker container exec`
to connect more terminals to it.

At the shell prompt in the container, you can use any of the tools listed
above.

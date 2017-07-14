# Vimol 2.0

![intro](intro.gif)

Vimol is a powerful molecular viewer and editor. Vimol features
vi-like key bindings, fast and lightweight design, editing in multiple tabs,
visualization of trajectories, unlimited undo/redo, command record/replay,
and much more!
Vimol does not require the mouse, and most things
can be accomplished in less than 5 keystrokes. Use **h**/**j**/**k**/**l**
keys to rotate the molecule and **q** to exit the program.
Multi-frame **pdb** and **xyz** file formats are supported for viewing
and editing.
For the detailed documentation consult the vimol(1) manual
page ([html](https://vimol.github.io/vimol.1.html),
[pdf](https://vimol.github.io/vimol.1.pdf)).


### Prerequisites

```
Cairo
SDL2
```


## Installation
### Command Line Interface

```sh
$ git clone https://github.com/ilyak/vimol.git
$ cd vimol
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./vimol
```

Use `vimol` to start the program, and `man vimol` to see the manual page.


## Authors

* **Ilya Kaliman**

# Introduction

`lines` is a small c-program for selecting lines from input file. The
input file is a disk file or stdin. The selected lines go to stdout.

The selection is performed by progressing through each line, using
commands, and selecting which lines are passed and which are rejected.
For example:

    lines m 10 p

will move (`m`, absolute) to line at index 10 and pass (`p`, pass) the
following lines until the end of file. The pass state is off, by
default, in the beginning of file. The first 10 lines of the file
would be displayed with:

    lines p j 10 r

`p` (pass) sets pass state on. `j` jumps (relative from current, i.e.
from 0 to 10) to index 10. `r` (reject) sets the pass state off. Line
indeces start from 0.

In addition to absolute and relative indeces, the line position can be
searched. With `s` command, the given tag is searched from the
beginning of line. If tag is found, the current line index is set.

Commands:

* `t`, toggle: Toggle output.

* `p`, pass: Enable output.

* `r`, reject: Disable output.

* `m <value>`, absolute: Move to line index.

* `j <value>`, relative: Jump relative to current line index.

* `f <tag>`, find, no skip: Search string and stop to matching line.

* `s <tag>`, find, with skip: Search string and jump over the matching
  line.

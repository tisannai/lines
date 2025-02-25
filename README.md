# Introduction

`lines` is a minimal c-program for selecting lines from input. The
input is a file or stdin. The selected lines go to stdout.

The selected lines are specified as a range (left and right limit).
Default is to select the lines within the inclusive range. Optionally
the range is exclusive. Right limit might be omitted and defaults to
input length.



Provides Java bindings for various native APIs.

# Available bindings

## Generic

* Get and set UNIX file mode.
* Get PID of current process.

## Terminal and console

These bindings work for both the UNIX terminal and Windows console:

* Determine if stdout/stderr are attached to a terminal.
* Query the terminal size.
* Switch between bold and normal mode on the terminal.
* Change foreground color on the terminal.
* Move terminal cursor up, down, left, right, start of line.
* Clear to end of line.

Currently ported to OS X, Linux and Windows. Tested on:

* OS X 10.7.4
* Ubunutu 12.04 (amd64)
* Windows 7 (amd64)

# Building

## Ubuntu

You need to install the `libncurses5-dev` package to pick up the ncurses header files. Also worth installing the `ncurses-doc` package too.

## Windows

You need to install Visual studio, and build from a Visual studio command prompt.

# TODO

* Fix TERM=dumb
* Split out separate native library for terminal handling.
* String names for errno values.
* Split into multiple projects.
* Handle multiple architectures.
* IBM JVM.
* Convert to c.
* Thread safety.
* Windows: flush System.out or System.err on attribute change.

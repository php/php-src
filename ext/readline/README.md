readline
========

Provides generic line editing, history, and tokenization functions.
See https://www.php.net/manual/en/book.readline.php

Implementation Details
----------------------

C variables starting with `rl_*` are declared by the readline library
(or are macros referring to variables from the libedit library).
See http://web.mit.edu/gnu/doc/html/rlman_2.html

This should only be used in the CLI SAPI.
Historically, the code lived in sapi/cli,
but many distributions build readline as a shared extension.
Therefore, that code was split into ext/readline so that this can dynamically
be loaded. With other SAPIs, readline is/should be disabled.

`readline_cli.c` implements most of the interactive shell(`php -a`).

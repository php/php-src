--TEST--
Test stdin input with strict types
--PHPDBG--
stdin foo
<?php
declare(strict_types=1);
echo "Hello, world!\n";
foo
b 3
r
c
r
q
--EXPECT--
prompt> [Successful compilation of stdin input]
prompt> [Breakpoint #0 added at Standard input code:3]
prompt> [Breakpoint #0 at Standard input code:3, hits: 1]
>00003: echo "Hello, world!\n";
 00004: 
prompt> Hello, world!
[Script ended normally]
prompt> [Breakpoint #0 at Standard input code:3, hits: 1]
>00003: echo "Hello, world!\n";
 00004: 
prompt> 

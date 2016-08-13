--TEST--
Test clearing breakpoints
--INI--
opcache.enable_cli=0
--PHPDBG--
b 4
b foo
r
clear
c
i b
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:4]
prompt> [Breakpoint #1 added at foo]
prompt> 1
[Breakpoint #0 at %s:4, hits: 1]
>00004: echo 2;
 00005: echo 3;
 00006: foo();
prompt> Clearing Breakpoints
File              1
Functions         1
Methods           0
Oplines           0
File oplines      0
Function oplines  0
Method oplines    0
Conditionals      0
prompt> 234
[Script ended normally]
prompt> prompt> 
--FILE--
<?php

echo 1;
echo 2;
echo 3;
foo();

function foo() {
	echo 4;
}

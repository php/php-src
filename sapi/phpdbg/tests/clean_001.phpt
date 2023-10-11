--TEST--
Cleaning must preserve breakpoints
--INI--
opcache.enable_cli=0
--PHPDBG--
b 4
b foo
r
c
clean
y
c
r
c

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
prompt> 23
[Breakpoint #1 in foo() at %s:9, hits: 1]
>00009: 	echo 4;
 00010: }
 00011: 
prompt> Do you really want to clean your current environment? (type y or n): Cleaning Execution Environment
Classes    %d
Functions  %d
Constants  %d
Includes   0
prompt> [Not running]
prompt> 1
[Breakpoint #0 at %s:4, hits: 1]
>00004: echo 2;
 00005: echo 3;
 00006: foo();
prompt> 23
[Breakpoint #1 in foo() at %s:9, hits: 1]
>00009: 	echo 4;
 00010: }
 00011: 
prompt> 4
[Script ended normally]
prompt> 
--FILE--
<?php

echo 1;
echo 2;
echo 3;
foo();

function foo() {
	echo 4;
}
?>

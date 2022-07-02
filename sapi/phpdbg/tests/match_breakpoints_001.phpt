--TEST--
Test match default breakpoint with variable assignment
--INI--
opcache.enable_cli=0
--PHPDBG--
b 5
b 10
r
q
--EXPECTF--
[Successful compilation of %s.php]
prompt> [Breakpoint #0 added at %s.php:5]
prompt> [Breakpoint #1 added at %s.php:10]
prompt> [Breakpoint #1 at %s.php:10, hits: 1]
>00010:     default => 'bar', // breakpoint #1
 00011: };
 00012: 
prompt>
--FILE--
<?php

$foo = match (0) {
    0 => 'foo',
    default => 'bar', // breakpoint #0
};

$foo = match (1) {
    0 => 'foo',
    default => 'bar', // breakpoint #1
};

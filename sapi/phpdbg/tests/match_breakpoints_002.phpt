--TEST--
Test match default breakpoint with property assignment
--INI--
opcache.enable_cli=0
--PHPDBG--
b 7
b 12
r
q
--EXPECTF--
[Successful compilation of %s.php]
prompt> [Breakpoint #0 added at %s.php:7]
prompt> [Breakpoint #1 added at %s.php:12]
prompt> [Breakpoint #1 at %s.php:12, hits: 1]
>00012:     default => 'bar', // breakpoint #1
 00013: };
 00014: 
prompt>
--FILE--
<?php

$foo = new stdClass();

$foo->bar = match (0) {
    0 => 'foo',
    default => 'bar', // breakpoint #0
};

$foo->bar = match (1) {
    0 => 'foo',
    default => 'bar', // breakpoint #1
};
?>

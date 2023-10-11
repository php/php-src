--TEST--
Test match default breakpoint with static variable assignment
--INI--
opcache.enable_cli=0
--PHPDBG--
b 9
b 14
r
q
--EXPECTF--
[Successful compilation of %s.php]
prompt> [Breakpoint #0 added at %s.php:9]
prompt> [Breakpoint #1 added at %s.php:14]
prompt> [Breakpoint #1 at %s.php:14, hits: 1]
>00014:     default => 'bar', // breakpoint #1
 00015: };
 00016: 
prompt>
--FILE--
<?php

class Foo {
    public static $bar;
}

Foo::$bar = match (0) {
    0 => 'foo',
    default => 'bar', // breakpoint #0
};

Foo::$bar = match (1) {
    0 => 'foo',
    default => 'bar', // breakpoint #1
};
?>

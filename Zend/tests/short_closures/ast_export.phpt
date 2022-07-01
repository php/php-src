--TEST--
Short closures: ast export
--INI--
zend.assertions=1
--FILE--
<?php

$a = 1;

assert(#[Attr] fn(DateTime $dt): DateTime {
    return 1;
} && false);

--EXPECTF--
Fatal error: Uncaught AssertionError: assert(#[Attr] fn(DateTime $dt): DateTime {
    return 1;
} && false) in %s:%d
Stack trace:
#0 %s(%d): assert(false, 'assert(#[Attr] ...')
#1 {main}
  thrown in %s on line %d

--TEST--
strict_types=0 code including strict_types=1 code
--FILE--
<?php

// implicitly weak code

// file with strict_types=1
require 'weak_include_strict_2.inc';

// calls within that file should stay strict, despite being included by weak file
?>
--EXPECTF--
Fatal error: Uncaught TypeError: takes_int(): Argument #1 ($x) must be of type int, float given, called in %s:%d
Stack trace:
#0 %s(%d): takes_int(1.0)
#1 %s(%d): require('%s')
#2 {main}
  thrown in %sweak_include_strict_2.inc on line 5

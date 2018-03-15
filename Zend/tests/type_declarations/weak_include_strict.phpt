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
Fatal error: Uncaught TypeError: Argument 1 passed to takes_int() must be of the type int, float given, called in %sweak_include_strict_2.inc on line 9 and defined in %sweak_include_strict_2.inc:5
Stack trace:
#0 %s(%d): takes_int(1)
#1 %s(%d): require('%s')
#2 {main}
  thrown in %sweak_include_strict_2.inc on line 5

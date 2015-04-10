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
Fatal error: Argument 1 passed to takes_int() must be of the type integer, float given, called in %sweak_include_strict_2.inc on line 9 and defined in %sweak_include_strict_2.inc on line 5

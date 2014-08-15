--TEST--
Simple math tests
--FILE--
<?php // $Id$

define('LONG_MAX64', 9223372036854775807);
define('LONG_MIN64', -LONG_MAX64 - 1);
define('LONG_MAX32', 0x7FFFFFFF);
define('LONG_MIN32', -LONG_MAX32 - 1);

$tests = <<<TESTS
 1   === abs(-1)
 1.5 === abs(-1.5)
 1   === abs("-1")
 1.5 === abs("-1.5")
-LONG_MIN32+1 === abs(LONG_MIN32-1)
-LONG_MIN32   === abs(LONG_MIN32)
-(LONG_MIN32+1) === abs(LONG_MIN32+1)
-LONG_MIN64+1 === abs(LONG_MIN64-1)
-LONG_MIN64   === abs(LONG_MIN64)
-(LONG_MIN64+1) === abs(LONG_MIN64+1)
TESTS;

include(dirname(__FILE__) . '/../../../../tests/quicktester.inc');
--EXPECT--
OK

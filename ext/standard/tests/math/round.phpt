--TEST--
Simple math tests
--FILE--
<?php // $Id$

define('LONG_MAX64', 9223372036854775807);
define('LONG_MIN64', -LONG_MAX64 - 1);
define('LONG_MAX32', 0x7FFFFFFF);
define('LONG_MIN32', -LONG_MAX32 - 1);

$tests = <<<TESTS
-1 ~== ceil(-1.5)
 2 ~== ceil( 1.5)
-2 ~== floor(-1.5)
 1 ~== floor(1.5)
(float) LONG_MIN64   ~== ceil(LONG_MIN64 - 0.5)
(float) LONG_MIN64+1 ~== ceil(LONG_MIN64 + 0.5)
(float) LONG_MIN64-1 ~== round(LONG_MIN64 - 0.6)
(float) LONG_MIN64   ~== round(LONG_MIN64 - 0.4)
(float) LONG_MIN64   ~== round(LONG_MIN64 + 0.4)
(float) LONG_MIN64+1 ~== round(LONG_MIN64 + 0.6)
(float) LONG_MIN64-1 ~== floor(LONG_MIN64 - 0.5)
(float) LONG_MIN64   ~== floor(LONG_MIN64 + 0.5)
(float) LONG_MAX64   ~== ceil(LONG_MAX64 - 0.5)
(float) LONG_MAX64+1 ~== ceil(LONG_MAX64 + 0.5)
(float) LONG_MAX64-1 ~== round(LONG_MAX64 - 0.6)
(float) LONG_MAX64   ~== round(LONG_MAX64 - 0.4)
(float) LONG_MAX64   ~== round(LONG_MAX64 + 0.4)
(float) LONG_MAX64+1 ~== round(LONG_MAX64 + 0.6)
(float) LONG_MAX64-1 ~== floor(LONG_MAX64 - 0.5)
(float) LONG_MAX64   ~== floor(LONG_MAX64 + 0.5)
(float) LONG_MIN32   ~== ceil(LONG_MIN32 - 0.5)
(float) LONG_MIN32+1 ~== ceil(LONG_MIN32 + 0.5)
(float) LONG_MIN32-1 ~== round(LONG_MIN32 - 0.6)
(float) LONG_MIN32   ~== round(LONG_MIN32 - 0.4)
(float) LONG_MIN32   ~== round(LONG_MIN32 + 0.4)
(float) LONG_MIN32+1 ~== round(LONG_MIN32 + 0.6)
(float) LONG_MIN32-1 ~== floor(LONG_MIN32 - 0.5)
(float) LONG_MIN32   ~== floor(LONG_MIN32 + 0.5)
(float) LONG_MAX32   ~== ceil(LONG_MAX32 - 0.5)
(float) LONG_MAX32+1 ~== ceil(LONG_MAX32 + 0.5)
(float) LONG_MAX32-1 ~== round(LONG_MAX32 - 0.6)
(float) LONG_MAX32   ~== round(LONG_MAX32 - 0.4)
(float) LONG_MAX32   ~== round(LONG_MAX32 + 0.4)
(float) LONG_MAX32+1 ~== round(LONG_MAX32 + 0.6)
(float) LONG_MAX32-1 ~== floor(LONG_MAX32 - 0.5)
(float) LONG_MAX32   ~== floor(LONG_MAX32 + 0.5)
TESTS;

include(dirname(__FILE__) . '/../../../../tests/quicktester.inc');
--EXPECT--
OK

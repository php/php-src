--TEST--
Bug #24054 (Assignment operator *= broken)
--FILE--
<?php // $Id$

define('LONG_MAX64', 9223372036854775807);
define('LONG_MAX32', 0x7FFFFFFF);

	$i = LONG_MAX64;

	$j = $i * 1001;
	$i *= 1001;

    $k = LONG_MAX32;
    
    $l = $k * 1001;
    $k *= 1001;
$tests = <<<TESTS
$i === $j
$l === $k
TESTS;

include(dirname(__FILE__) . '/../quicktester.inc');

--EXPECT--
OK
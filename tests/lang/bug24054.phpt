--TEST--
Bug #24054 (Assignment operator *= broken)
--FILE--
<?php // $Id$

define('LONG_MAX', is_int(5000000000)? 9223372036854775807 : 0x7FFFFFFF);
define('LONG_MIN', -LONG_MAX - 1);
printf("%d,%d,%d,%d\n",is_int(LONG_MIN  ),is_int(LONG_MAX  ),
					   is_int(LONG_MIN-1),is_int(LONG_MAX+1));

	$i = LONG_MAX;

	$j = $i * 1001;
	$i *= 1001;

$tests = <<<TESTS
$i === $j
TESTS;

include(dirname(__FILE__) . '/../quicktester.inc');
--EXPECT--
1,1,0,0
OK

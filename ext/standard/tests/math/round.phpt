--TEST--
Simple math tests
--FILE--
<?php

define('LONG_MAX', is_int(5000000000)? 9223372036854775807 : 0x7FFFFFFF);
define('LONG_MIN', -LONG_MAX - 1);
printf("%d,%d,%d,%d\n",is_int(LONG_MIN  ),is_int(LONG_MAX  ),
                       is_int(LONG_MIN-1),is_int(LONG_MAX+1));

$tests = <<<TESTS
-1 ~== ceil(-1.5)
 2 ~== ceil( 1.5)
-2 ~== floor(-1.5)
 1 ~== floor(1.5)
 LONG_MIN   ~== ceil(LONG_MIN - 0.5)
 LONG_MIN+1 ~== ceil(LONG_MIN + 0.5)
 LONG_MIN-1 ~== round(LONG_MIN - 0.6)
 LONG_MIN   ~== round(LONG_MIN - 0.4)
 LONG_MIN   ~== round(LONG_MIN + 0.4)
 LONG_MIN+1 ~== round(LONG_MIN + 0.6)
 LONG_MIN-1 ~== floor(LONG_MIN - 0.5)
 LONG_MIN   ~== floor(LONG_MIN + 0.5)
 LONG_MAX   ~== ceil(LONG_MAX - 0.5)
 LONG_MAX+1 ~== ceil(LONG_MAX + 0.5)
 LONG_MAX-1 ~== round(LONG_MAX - 0.6)
 LONG_MAX   ~== round(LONG_MAX - 0.4)
 LONG_MAX   ~== round(LONG_MAX + 0.4)
 LONG_MAX+1 ~== round(LONG_MAX + 0.6)
 LONG_MAX-1 ~== floor(LONG_MAX - 0.5)
 LONG_MAX   ~== floor(LONG_MAX + 0.5)
TESTS;

include(__DIR__ . '/../../../../tests/quicktester.inc');
?>
--EXPECT--
1,1,0,0
OK

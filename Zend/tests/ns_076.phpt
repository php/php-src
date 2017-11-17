--TEST--
076: Unknown constants in namespace
--FILE--
<?php
namespace foo;

$a = array(unknown => unknown);

echo unknown;
echo "\n";
var_dump($a);
echo \unknown;
--EXPECTF--
Warning: Use of undefined constant unknown - assumed 'unknown' (this will throw an Error in a future version of PHP) in %sns_076.php on line %d

Warning: Use of undefined constant unknown - assumed 'unknown' (this will throw an Error in a future version of PHP) in %sns_076.php on line %d

Warning: Use of undefined constant unknown - assumed 'unknown' (this will throw an Error in a future version of PHP) in %sns_076.php on line %d
unknown
array(1) {
  ["unknown"]=>
  %s(7) "unknown"
}

Fatal error: Uncaught Error: Undefined constant 'unknown' in %sns_076.php:%d
Stack trace:
#0 {main}
  thrown in %sns_076.php on line %d

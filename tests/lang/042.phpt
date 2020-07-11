--TEST--
Dynamic access of constants
--FILE--
<?php
class A {
    const B = 'foo';
}

$classname       =  'A';
$wrongClassname  =  'B';

echo $classname::B."\n";
echo $wrongClassname::B."\n";
?>
===DONE===
--EXPECTF--
foo

Fatal error: Uncaught Error: Class "B" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s042.php on line %d

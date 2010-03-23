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

Fatal error: Class 'B' not found in %s042.php on line %d

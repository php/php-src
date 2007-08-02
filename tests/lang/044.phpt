--TEST--
Dynamic call for static methods dynamically named
--FILE--
<?php
class A {
    static function foo() { return 'foo'; }
}
$classname        =  'A';
$binaryClassname  = b'A';
$wrongClassname   =  'B';

$methodname       =  'foo';
$binaryMethodname = b'foo';

echo $classname::$methodname()."\n";
echo $classname::$binaryMethodname()."\n";

echo $binaryClassname::$methodname()."\n";
echo $binaryClassname::$binaryMethodname()."\n";

echo $wrongClassname::$binaryMethodname()."\n";
?> 
===DONE===
--EXPECTF--
foo
foo
foo
foo

Fatal error: Class 'B' not found in %s044.php on line %d

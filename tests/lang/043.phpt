--TEST--
Dynamic call for static methods
--FILE--
<?php
class A {
    static function foo() { return 'foo'; }
}

$classname       =  'A';
$binaryClassname = b'A';
$wrongClassname  =  'B';

echo $classname::foo()."\n";
echo $binaryClassname::foo()."\n";
echo $wrongClassname::foo()."\n";
?> 
===DONE===
--EXPECTF--
foo
foo

Fatal error: Class 'B' not found in %s043.php on line %d

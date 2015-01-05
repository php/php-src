--TEST--
Dynamic call for static methods dynamically named
--FILE--
<?php
class A {
    static function foo() { return 'foo'; }
}
$classname        =  'A';
$wrongClassname   =  'B';

$methodname       =  'foo';

echo $classname::$methodname()."\n";

echo $wrongClassname::$methodname()."\n";
?>
===DONE===
--EXPECTF--
foo

Fatal error: Class 'B' not found in %s044.php on line %d

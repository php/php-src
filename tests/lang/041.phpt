--TEST--
Dynamic access of static members
--FILE--
<?php
class A {
    public    static $b = 'foo';
}

$classname       =  'A';
$wrongClassname  =  'B';

echo $classname::$b."\n";
echo $wrongClassname::$b."\n";

?>
===DONE===
--EXPECTF--
foo

Fatal error: Class 'B' not found in %s041.php on line %d

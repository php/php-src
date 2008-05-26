--TEST--
Bug #28442 (Changing a static variables in a class changes it across sub/super classes.)
--FILE--
<?php

class ClassA
{
   static $prop;
}

class ClassB extends ClassA
{
   static $prop;
}

class ClassC extends ClassB
{
}

echo "===INIT===\n";
ClassA::$prop = 'A';
ClassB::$prop = 'B';
ClassC::$prop = 'C';
var_dump(ClassA::$prop);
var_dump(ClassB::$prop);
var_dump(ClassC::$prop);

echo "===SetA===\n";
ClassA::$prop = 'A2';
var_dump(ClassA::$prop);
var_dump(ClassB::$prop);
var_dump(ClassC::$prop);

echo "===SetB===\n";
ClassB::$prop = 'B2';
var_dump(ClassA::$prop);
var_dump(ClassB::$prop);
var_dump(ClassC::$prop);

echo "===SetC===\n";
ClassC::$prop = 'C2';
var_dump(ClassA::$prop);
var_dump(ClassB::$prop);
var_dump(ClassC::$prop);

?>
===DONE===
--EXPECT--
===INIT===
unicode(1) "A"
unicode(1) "C"
unicode(1) "C"
===SetA===
unicode(2) "A2"
unicode(1) "C"
unicode(1) "C"
===SetB===
unicode(2) "A2"
unicode(2) "B2"
unicode(2) "B2"
===SetC===
unicode(2) "A2"
unicode(2) "C2"
unicode(2) "C2"
===DONE===

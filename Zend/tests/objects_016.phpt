--TEST--
Strict error when assigning value to property of inexistent object
--FILE--
<?php

error_reporting(E_ALL);

echo "\nAbout to create a default object from empty value:\n";
$obj1->p = 1;
echo "\nAbout to create a default object from empty value again:\n";
$obj2->p[] = 1;
echo "\nAbout to create a default object from empty value one more time:\n";
$a = 1;
$obj3->p = &$a;

?>
--EXPECTF--
About to create a default object from empty value:

Strict Standards: Creating default object from empty value in %s on line %d

About to create a default object from empty value again:

Strict Standards: Creating default object from empty value in %s on line %d

About to create a default object from empty value one more time:

Strict Standards: Creating default object from empty value in %s on line %d

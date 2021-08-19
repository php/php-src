--TEST--
Test standard 'is_equal' object handler
--FILE--
<?php

echo "Simple test for standard is_equal object handler\n";

class class1{}

class class2{}

class class3{
    public $aaa;
    private $bbb;
    protected $ccc;
}

class class4 extends class3{
}

class class5 extends class3{
    public $ddd;
    private $eee;
}

// Define a bunch of objects all of which will use standard compare object handler
$obj1 = new class1();
$obj2 = new class2();
$obj3 = new class3();
$obj4 = new class4();
$obj5 = new class5();

echo "\n-- The following is_equal should return TRUE --\n";
var_dump($obj1 == $obj1);

echo "\n-- All the following is_equals should throw InvalidOperator --\n";
try {
    $obj1 == $obj2;
} catch (InvalidOperator) {
    echo "InvalidOperator thrown".PHP_EOL;
}
try {
    $obj1 == $obj3;
} catch (InvalidOperator) {
    echo "InvalidOperator thrown".PHP_EOL;
}
try {
    $obj1 == $obj4;
} catch (InvalidOperator) {
    echo "InvalidOperator thrown".PHP_EOL;
}
try {
    $obj1 == $obj5;
} catch (InvalidOperator) {
    echo "InvalidOperator thrown".PHP_EOL;
}
try {
    $obj4 == $obj3;
} catch (InvalidOperator) {
    echo "InvalidOperator thrown".PHP_EOL;
}
try {
    $obj5 == $obj3;
} catch (InvalidOperator) {
    echo "InvalidOperator thrown".PHP_EOL;
}

?>
--EXPECT--
Simple test for standard is_equal object handler

-- The following is_equal should return TRUE --
bool(true)

-- All the following is_equals should throw InvalidOperator --
InvalidOperator thrown
InvalidOperator thrown
InvalidOperator thrown
InvalidOperator thrown
InvalidOperator thrown
InvalidOperator thrown

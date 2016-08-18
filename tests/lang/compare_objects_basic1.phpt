--TEST--
Test standard 'compare' object handler 
--FILE--

<?php

echo "Simple test for standard compare object handler\n";

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

echo "\n-- The following compare should return TRUE --\n"; 
var_dump($obj1 == $obj1);

echo "\n-- All the following compares should return FALSE --\n"; 
var_dump($obj1 == $obj2);
var_dump($obj1 == $obj3);
var_dump($obj1 == $obj4);
var_dump($obj1 == $obj5);
var_dump($obj4 == $obj3);
var_dump($obj5 == $obj3);

?>
===DONE===
--EXPECT--
Simple test for standard compare object handler

-- The following compare should return TRUE --
bool(true)

-- All the following compares should return FALSE --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
===DONE===

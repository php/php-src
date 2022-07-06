--TEST--
Test of compare object handler for DateTime objects
--FILE--
<?php

echo "Simple test for DateTime compare object handler\n";

//Set the default time zone
date_default_timezone_set("Europe/London");

class DateTimeExt1 extends DateTime {
}

class DateTimeExt2 extends DateTime{
    public $foo = "Hello";
    private $bar = 99;
}

class DateTimeExt3 extends DateTimeExt2 {
}

$obj1 = new DateTime("2009-02-12 12:47:41 GMT");
$obj2 = new DateTimeExt1("2009-02-12 12:47:41 GMT");
$obj3 = new DateTimeExt2("2009-02-12 12:47:41 GMT");
$obj4 = new DateTimeExt3("2009-02-12 12:47:41 GMT");

echo "\n-- All the following tests should compare equal --\n";
var_dump($obj1 == $obj1);
var_dump($obj1 == $obj2);
var_dump($obj1 == $obj3);
var_dump($obj1 == $obj4);
var_dump($obj2 == $obj3);
var_dump($obj2 == $obj4);
var_dump($obj3 == $obj4);

date_modify($obj1, "+1 day");
echo "\n-- The following test should still compare equal --\n";
var_dump($obj1 == $obj1);
echo "\n-- All the following tests should now compare NOT equal --\n";
var_dump($obj1 == $obj2);
var_dump($obj1 == $obj3);
var_dump($obj1 == $obj4);

echo "\n-- All the following tests should again compare equal --\n";
date_modify($obj2, "+1 day");
date_modify($obj3, "+1 day");
date_modify($obj4, "+1 day");
var_dump($obj1 == $obj2);
var_dump($obj1 == $obj3);
var_dump($obj1 == $obj4);
?>
--EXPECT--
Simple test for DateTime compare object handler

-- All the following tests should compare equal --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

-- The following test should still compare equal --
bool(true)

-- All the following tests should now compare NOT equal --
bool(false)
bool(false)
bool(false)

-- All the following tests should again compare equal --
bool(true)
bool(true)
bool(true)

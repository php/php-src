--TEST--
Test object compare when object handler different 
--FILE--

<?php 

//Set the default time zone 
date_default_timezone_set("Europe/London");

echo "Simple test comparing two objects with different compare callback handler\n";

class X {
}

$obj1 = new X();
$obj2 = new DateTime(("2009-02-12 12:47:41 GMT"));

var_dump($obj1 == $obj2);
?>
===DONE===
--EXPECTF--
Simple test comparing two objects with different compare callback handler
bool(false)
===DONE===

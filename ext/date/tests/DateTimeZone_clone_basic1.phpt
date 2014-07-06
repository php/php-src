--TEST--
Test clone on DateTimeZone objects
--FILE--
<?php

//Set the default time zone 
date_default_timezone_set('Europe/London');

echo "*** Testing clone on DateTime objects ***\n";

// Create a DateTimeZone object..
$orig =  new DateTimeZone("GMT");

// ..create a clone of it ..Clone 
$clone = clone $orig;

var_dump($orig);
var_dump($clone); 

if ($clone != $orig) { 
	echo "TEST FAILED : objects not equal\n"; 
}else if ($clone === $orig) { 
	echo "TEST FAILED : objects identical\n"; 
} else {
	echo "TEST PASSED : Objects equal but not indetical\n"; 
}		

?>
===DONE===
--EXPECTF--
*** Testing clone on DateTime objects ***
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "GMT"
}
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "GMT"
}
TEST PASSED : Objects equal but not indetical
===DONE===



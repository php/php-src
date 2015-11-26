--TEST--
assigning static property as non static
--INI--
error_reporting=8191
--FILE--
<?php

class test { 
	static $foo = 1;
} 

$t = new test; 
$t->foo = 5;

$fp = fopen(__FILE__, 'r');

var_dump($t);

echo "Done\n";
?>
--EXPECTF--	
Notice: Accessing static property test::$foo as non static in %s on line %d
object(test)#%d (1) {
  ["foo"]=>
  int(5)
}
Done

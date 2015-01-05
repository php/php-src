--TEST--
array_map() and exceptions in the callback
--FILE--
<?php

$a = array(1,2,3);

function foo() { 
	throw new exception(1); 
} 

try { 
	array_map("foo", $a, array(2,3)); 
} catch (Exception $e) {
	var_dump("exception caught!");
}

echo "Done\n";
?>
--EXPECTF--	
Warning: array_map(): An error occurred while invoking the map callback in %s on line %d
string(17) "exception caught!"
Done

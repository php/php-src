--TEST--
get_resource_id() function
--FILE--
<?php

$file = fopen(__FILE__, 'r');

// get_resource_id() is equivalent to an integer cast.
var_dump(get_resource_id($file) === (int) $file);

// Also works with closed resources.
fclose($file);
var_dump(get_resource_id($file) === (int) $file);

?>
--EXPECT--
bool(true)
bool(true)

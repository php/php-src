--TEST--
Test serialize() & unserialize() functions: resources
--FILE--
<?php
echo "\n--- Testing Resource ---\n";
$file_handle = fopen( __FILE__, "r" );
$serialized_data = serialize( $file_handle );
fclose($file_handle);
var_dump($serialized_data);
var_dump(unserialize($serialized_data));

echo "\nDone";
?>
--EXPECTF--
--- Testing Resource ---
string(4) "i:%d;"
int(%d)

Done

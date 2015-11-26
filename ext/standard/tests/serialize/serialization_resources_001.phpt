--TEST--
Test serialize() & unserialize() functions: resources
--FILE--
<?php 
/* Prototype  : proto string serialize(mixed variable)
 * Description: Returns a string representation of variable (which can later be unserialized) 
 * Source code: ext/standard/var.c
 * Alias to functions: 
 */
/* Prototype  : proto mixed unserialize(string variable_representation)
 * Description: Takes a string representation of variable and recreates it 
 * Source code: ext/standard/var.c
 * Alias to functions: 
 */

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
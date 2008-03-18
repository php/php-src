--TEST--
Ensure __autoload is called twice if unserialize_callback_func is defined.
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

function __autoload($name) {
	echo "in __autoload($name)\n";
}

ini_set('unserialize_callback_func','check');

function check($name) {
	echo "in check($name)\n"; 
}

$o = unserialize('O:3:"FOO":0:{}');

var_dump($o);

echo "Done";
?>
--EXPECTF--
in __autoload(FOO)
in check(FOO)
in __autoload(FOO)

Warning: unserialize(): Function check() hasn't defined the class it was called for in %s on line 23
object(__PHP_Incomplete_Class)#%d (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(3) "FOO"
}
Done
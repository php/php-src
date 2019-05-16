--TEST--
Bad unserialize_callback_func
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

ini_set('unserialize_callback_func','Nonexistent');
$o = unserialize('O:3:"FOO":0:{}');
var_dump($o);
echo "Done";
?>
--EXPECTF--
Warning: unserialize(): defined (Nonexistent) but not found in %s on line 14
object(__PHP_Incomplete_Class)#%d (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(3) "FOO"
}
Done

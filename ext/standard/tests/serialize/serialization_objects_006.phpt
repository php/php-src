--TEST--
Behaviour of incomplete class is preserved even when it was not created by unserialize().
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

$a = new __PHP_Incomplete_Class;
var_dump($a);
var_dump($a->p);

echo "Done";
?>
--EXPECTF--
object(__PHP_Incomplete_Class)#%d (0) {
}

Notice: main(): The script tried to execute a method or access a property of an incomplete object. Please ensure that the class definition "unknown" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide a __autoload() function to load the class definition  in %s on line 15
NULL
Done
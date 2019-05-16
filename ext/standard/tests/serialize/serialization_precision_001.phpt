--TEST--
Test serialize_precision (part 1)
--INI--
serialize_precision=10
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

var_dump(serialize(0.1));
?>
--EXPECT--
string(6) "d:0.1;"

--TEST--
Custom unserialization of classes with no custom unserializer.
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

$ser = 'C:1:"C":6:{dasdas}';
$a = unserialize($ser);
eval('class C {}');
$b = unserialize($ser);

var_dump($a, $b);

echo "Done";
?>
--EXPECTF--
Warning: Class __PHP_Incomplete_Class has no unserializer in %sserialization_objects_009.php on line %d

Warning: Class C has no unserializer in %sserialization_objects_009.php on line %d
object(__PHP_Incomplete_Class)#%d (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(1) "C"
}
object(C)#%d (0) {
}
Done

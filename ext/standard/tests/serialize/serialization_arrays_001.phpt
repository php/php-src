--TEST--
Test serialize() & unserialize() functions: arrays (circular references)
--INI--
serialize_precision=100
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

echo "\n--- Testing Circular reference of an array ---\n";

echo "-- Normal array --\n";
$arr_circ = array(0, 1, -2, 3.333333, "a", array(), &$arr_circ);
$serialize_data = serialize($arr_circ);
var_dump( $serialize_data );
$arr_circ = unserialize($serialize_data);
var_dump( $arr_circ );

echo "\n-- Associative array --\n";
$arr_asso = array("a" => "test");
$arr_asso[ "b" ] = &$arr_asso[ "a" ];
var_dump($arr_asso);
$serialize_data = serialize($arr_asso);
var_dump($serialize_data);
$arr_asso = unserialize($serialize_data);
var_dump($arr_asso);

echo "\nDone";
?>
--EXPECTF--
--- Testing Circular reference of an array ---
-- Normal array --
string(238) "a:7:{i:0;i:0;i:1;i:1;i:2;i:-2;i:3;d:3.333333000000000101437080957111902534961700439453125;i:4;s:1:"a";i:5;a:0:{}i:6;a:7:{i:0;i:0;i:1;i:1;i:2;i:-2;i:3;d:3.333333000000000101437080957111902534961700439453125;i:4;s:1:"a";i:5;a:0:{}i:6;R:8;}}"
array(7) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(-2)
  [3]=>
  float(3.333333)
  [4]=>
  string(1) "a"
  [5]=>
  array(0) {
  }
  [6]=>
  &array(7) {
    [0]=>
    int(0)
    [1]=>
    int(1)
    [2]=>
    int(-2)
    [3]=>
    float(3.333333)
    [4]=>
    string(1) "a"
    [5]=>
    array(0) {
    }
    [6]=>
    *RECURSION*
  }
}

-- Associative array --
array(2) {
  ["a"]=>
  &string(4) "test"
  ["b"]=>
  &string(4) "test"
}
string(37) "a:2:{s:1:"a";s:4:"test";s:1:"b";R:2;}"
array(2) {
  ["a"]=>
  &string(4) "test"
  ["b"]=>
  &string(4) "test"
}

Done

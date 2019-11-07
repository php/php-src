--TEST--
Test array_reduce() function : variation
--FILE--
<?php

/* Prototype  : mixed array_reduce(array input, mixed callback [, int initial])
 * Description: Iteratively reduce the array to a single value via the callback.
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

echo "*** Testing array_reduce() : variation - key argument on callback ***\n";

echo "\n--- Testing array_reduce() with long key argument  ---\n";
$array = ["nyan", "lamp", "binary", "php", "elephant"];

function reduce_long_key($v, $w, $x){
    return "$v,$x-$w";
}
var_dump(array_reduce($array, 'reduce_long_key'));

echo "\n--- Testing array_reduce() with string key argument  ---\n";
$array = ["nyan" => "nyan", "lamp" => "lamp", "binary" =>  "binary", "php" => "php", "elephant" => "elephant"];

function reduce_string_key($v, $w, $x){
    return "$v,$x-$w";
}
var_dump(array_reduce($array, 'reduce_string_key'));

?>
===DONE===
--EXPECT--
*** Testing array_reduce() : variation - key argument on callback ***

--- Testing array_reduce() with long key argument  ---
string(40) ",0-nyan,1-lamp,2-binary,3-php,4-elephant"

--- Testing array_reduce() with string key argument  ---
string(60) ",nyan-nyan,lamp-lamp,binary-binary,php-php,elephant-elephant"
===DONE===

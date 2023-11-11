--TEST--
JIT ASSIGN_DIM_OP: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--EXTENSIONS--
opcache
--FILE--
<?php
function test1() {
    $x = "a";
    $a[$x . "b"] = 0;
    $a[$x . "b"] += 2;
    var_dump($a);
}
test1();

function false_to_array($a) {
    $a[2] += 1;
    return $a;
}
function false_to_array_append($a) {
    $a[] += 1;
    return $a;
}
function false_to_array_invalid_index($a) {
    var_dump($a[[]] += 1);
    return $a;
}
function false_to_array_nested($a) {
    $a[2][3] += 1;
    return $a;
}
function false_to_array_nested_append($a) {
    $a[][] += 1;
    return $a;
}
function false_to_array_nested_invalid_index($a) {
    $a[[]][0] += 1;
    return $a;
}
function modulo_string($a) {
    $a[] %= "";
}

false_to_array(false);
false_to_array_append(false);
try {
    var_dump(false_to_array_invalid_index(false));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump(false_to_array_nested(false));
var_dump(false_to_array_nested_append(false));
try {
    var_dump(false_to_array_nested_invalid_index(false));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(modulo_string([]));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
array(1) {
  ["ab"]=>
  int(2)
}

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d

Warning: Undefined array key 2 in %s on line %d

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d
Illegal offset type

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d

Warning: Undefined array key 2 in %s on line %d

Warning: Undefined array key 3 in %s on line %d
array(1) {
  [2]=>
  array(1) {
    [3]=>
    int(1)
  }
}

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d
Illegal offset type
Unsupported operand types: null % string

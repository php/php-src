--TEST--
Test str_replace() function and array refs
--INI--
precision=14
--FILE--
<?php
$data = ['a' => 'b', 'numeric' => 1];
$ref = &$data;
$b = &$ref['a'];
$numeric = &$ref['numeric'];
var_dump(str_replace(array_keys($data), $data, "a numeric"));
var_dump($numeric);
var_dump($data['numeric']);
--EXPECTF--
string(3) "b 1"
int(1)
int(1)

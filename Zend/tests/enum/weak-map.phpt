--TEST--
Use enum as WeakMap key
--FILE--
<?php

enum TestEnum {
    case A;
}

$map = new WeakMap();
$map[TestEnum::A] = 'a string';
var_dump($map[TestEnum::A]);
var_dump($map[TestEnum::A]);

?>
--EXPECT--
string(8) "a string"
string(8) "a string"

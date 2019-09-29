--TEST--
mb_convert_encoding() shouldn't leak keys
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php

$x = "x";
$array = ["foo" . $x => "bar"];
mb_convert_encoding($array, 'UTF-8', 'UTF-8');
var_dump($array);

?>
--EXPECT--
array(1) {
  ["foox"]=>
  string(3) "bar"
}

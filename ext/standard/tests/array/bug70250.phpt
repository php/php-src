--TEST--
Bug #70250 (extract() turns array elements to references)
--FILE--
<?php
$array = ['key' => 'value'];

$ref = &$array['key'];

unset($ref);

extract($array);

$key = "bad";

var_dump($array);
?>
--EXPECT--
array(1) {
  ["key"]=>
  string(5) "value"
}

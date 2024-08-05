--TEST--
xxh3 convert secret to string should not modify (shm) array
--FILE--
<?php
try {
	hash_init("xxh3", options: $x = ["secret" => 4]);
} catch (Throwable) {}
var_dump($x);
?>
--EXPECT--
array(1) {
  ["secret"]=>
  int(4)
}

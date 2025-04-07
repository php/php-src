--TEST--
xxh3 convert secret to string should not modify (shm) array
--FILE--
<?php
try {
	hash_init("xxh3", options: $x = ["secret" => 4]);
} catch (Throwable) {}
var_dump($x);
?>
--EXPECTF--
Deprecated: hash_init(): Passing a secret of a type other than string is deprecated because it implicitly converts to a string, potentially hiding bugs in %s on line %d
array(1) {
  ["secret"]=>
  int(4)
}

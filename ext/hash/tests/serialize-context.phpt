--TEST--
Serialize a context
--SKIPIF--
<?php if (!extension_loaded('hash')) echo 'skip';
--FILE--
<?php

$h = hash_init('md5');
try {
  var_dump(serialize($h));
} catch (Exception $e) {
  echo "Exception: {$e->getMessage()}\n";
}
--EXPECT--
Exception: Serialization of 'HashContext' is not allowed

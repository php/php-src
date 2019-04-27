--TEST--
Hash: Context serialization
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

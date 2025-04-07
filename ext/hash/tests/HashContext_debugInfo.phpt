--TEST--
Hash: HashContext::__debugInfo()
--FILE--
<?php

var_dump(hash_init('sha256'));
var_dump(hash_init('sha3-512'));

?>
--EXPECTF--
object(HashContext)#%d (1) {
  ["algo"]=>
  string(6) "sha256"
}
object(HashContext)#%d (1) {
  ["algo"]=>
  string(8) "sha3-512"
}

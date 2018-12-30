--TEST--
Bug #77370 (Buffer overflow on mb regex functions - fetch_token)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump(mb_split("   \xfd",""));
?>
--EXPECT--
array(1) {
  [0]=>
  string(0) ""
}

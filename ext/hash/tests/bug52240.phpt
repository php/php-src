--TEST--
Bug #52240 (hash_copy() does not copy the HMAC key, causes wrong results and PHP crashes)
--SKIPIF--
<?php extension_loaded('hash') or die('skip'); ?>
--FILE--
<?php

$h = hash_init('crc32b', HASH_HMAC, '123456' );
$h2 = hash_copy($h);
var_dump(hash_final($h));
$h3 = hash_copy($h2);
var_dump(hash_final($h2));
var_dump(hash_final($h3));

?>
--EXPECT--
string(8) "278af264"
string(8) "278af264"
string(8) "278af264"

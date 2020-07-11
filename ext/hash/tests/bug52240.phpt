--TEST--
Bug #52240 (hash_copy() does not copy the HMAC key, causes wrong results and PHP crashes)
--FILE--
<?php

$h = hash_init('md5', HASH_HMAC, '123456');
$h2 = hash_copy($h);
var_dump(hash_final($h));
$h3 = hash_copy($h2);
var_dump(hash_final($h2));
var_dump(hash_final($h3));

?>
--EXPECT--
string(32) "cab1380ea86d8acc9aa62390a58406aa"
string(32) "cab1380ea86d8acc9aa62390a58406aa"
string(32) "cab1380ea86d8acc9aa62390a58406aa"

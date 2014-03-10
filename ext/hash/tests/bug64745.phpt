--TEST--
Bug #64745 hash_pbkdf2() truncates data when using default length and hex output
--SKIPIF--
<?php extension_loaded('hash') or die('skip'); ?>
--FILE--
<?php
$hash = hash_pbkdf2('sha1', 'password', 'salt', 1, 0);
$rawHash = hash_pbkdf2('sha1', 'password', 'salt', 1, 0, true);

var_dump($hash);
var_dump(bin2hex($rawHash));

?>
--EXPECT--
string(40) "0c60c80f961f0e71f3a9b524af6012062fe037a6"
string(40) "0c60c80f961f0e71f3a9b524af6012062fe037a6"


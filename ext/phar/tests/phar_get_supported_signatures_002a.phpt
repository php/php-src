--TEST--
Phar::getSupportedSignatures()
--EXTENSIONS--
phar
--SKIPIF--
<?php
$arr = Phar::getSupportedSignatures();
if (!in_array("OpenSSL", $arr)) die("skip openssl support required");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
var_dump(Phar::getSupportedSignatures());
?>
--EXPECT--
array(7) {
  [0]=>
  string(3) "MD5"
  [1]=>
  string(5) "SHA-1"
  [2]=>
  string(7) "SHA-256"
  [3]=>
  string(7) "SHA-512"
  [4]=>
  string(7) "OpenSSL"
  [5]=>
  string(14) "OpenSSL_SHA256"
  [6]=>
  string(14) "OpenSSL_SHA512"
}

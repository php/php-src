--TEST--
Phar::getSupportedSignatures()
--EXTENSIONS--
phar
--SKIPIF--
<?php
$arr = Phar::getSupportedSignatures();
if (in_array("OpenSSL", $arr)) die("skip openssl support enabled");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
var_dump(Phar::getSupportedSignatures());
?>
--EXPECT--
array(4) {
  [0]=>
  string(3) "MD5"
  [1]=>
  string(5) "SHA-1"
  [2]=>
  string(7) "SHA-256"
  [3]=>
  string(7) "SHA-512"
}

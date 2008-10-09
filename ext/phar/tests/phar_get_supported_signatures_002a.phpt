--TEST--
Phar::getSupportedSignatures()
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (!extension_loaded("hash")) die("skip extension hash required");
$arr = Phar::getSupportedSignatures();
if (!in_array("OpenSSL", $arr)) die("skip openssl support required");
if (!in_array('SHA-256', $arr)) die("skip hash extension loaded shared");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
var_dump(Phar::getSupportedSignatures());
?>
===DONE===
?>
--EXPECT--
array(5) {
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
}
===DONE===

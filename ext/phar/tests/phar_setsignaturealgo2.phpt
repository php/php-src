--TEST--
Phar::setSupportedSignatures() with hash
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (!extension_loaded("hash")) die("skip hash extension required");
$arr = Phar::getSupportedSignatures();
if (!in_array("OpenSSL", $arr)) die("skip openssl support required");
if (!in_array('SHA-256', $arr)) die("skip hash extension loaded shared");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar';
$p = new Phar($fname);
$p['file1.txt'] = 'hi';
var_dump($p->getSignature());
$p->setSignatureAlgorithm(Phar::MD5);
var_dump($p->getSignature());
$p->setSignatureAlgorithm(Phar::SHA1);
var_dump($p->getSignature());
try {
$p->setSignatureAlgorithm(Phar::SHA256);
var_dump($p->getSignature());
} catch (Exception $e) {
echo $e->getMessage();
}
try {
$p->setSignatureAlgorithm(Phar::SHA512);
var_dump($p->getSignature());
} catch (Exception $e) {
echo $e->getMessage();
}
try {
$config = dirname(__FILE__) . '/files/openssl.cnf';
$config_arg = array('config' => $config);
$private = openssl_get_privatekey(file_get_contents(dirname(__FILE__) . '/files/private.pem'));
$pkey = '';
openssl_pkey_export($private, $pkey, NULL, $config_arg);
$p->setSignatureAlgorithm(Phar::OPENSSL, $pkey);
var_dump($p->getSignature());
} catch (Exception $e) {
echo $e->getMessage();
}
?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar');
?>
--EXPECTF--
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(5) "SHA-1"
}
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(3) "MD5"
}
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(5) "SHA-1"
}
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(7) "SHA-256"
}
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(7) "SHA-512"
}
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(7) "OpenSSL"
}
===DONE===

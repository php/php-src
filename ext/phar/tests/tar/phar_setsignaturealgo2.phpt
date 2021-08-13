--TEST--
Phar::setSupportedSignatures() with hash, tar-based
--EXTENSIONS--
phar
--SKIPIF--
<?php
$arr = Phar::getSupportedSignatures();
if (!in_array("OpenSSL", $arr)) die("skip openssl support required");
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar';
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
$config = __DIR__ . '/../files/openssl.cnf';
$config_arg = array('config' => $config);
$private = openssl_get_privatekey(file_get_contents(dirname(__DIR__) . '/files/private.pem'));
$pkey = '';
openssl_pkey_export($private, $pkey, NULL, $config_arg);
$p->setSignatureAlgorithm(Phar::OPENSSL, $pkey);
var_dump($p->getSignature());
$p->setSignatureAlgorithm(Phar::OPENSSL_SHA512, $pkey);
var_dump($p->getSignature());
$p->setSignatureAlgorithm(Phar::OPENSSL_SHA256, $pkey);
var_dump($p->getSignature());
} catch (Exception $e) {
echo $e->getMessage();
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
?>
--EXPECTF--
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
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(14) "OpenSSL_SHA512"
}
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(14) "OpenSSL_SHA256"
}

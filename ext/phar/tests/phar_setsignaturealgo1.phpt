--TEST--
Phar::setSignatureAlgorithm()
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if ( extension_loaded("hash")) die("skip extension hash conflicts"); ?>
<?php if (!defined("Phar::PGP")) die("skip PGP Signature algorithm not available"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$p = new Phar(dirname(__FILE__) . '/brandnewphar.phar', 0, 'brandnewphar.phar');
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
$p->setSignatureAlgorithm(Phar::PGP);
var_dump($p->getSignature());
} catch (Exception $e) {
echo $e->getMessage();
}
?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/brandnewphar.phar');
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
string (82) "SHA-256 and SHA-512 signatures are only supported if the hash extension is enabled"
string (82) "SHA-256 and SHA-512 signatures are only supported if the hash extension is enabled"
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(5) "SHA-1"
}
===DONE===

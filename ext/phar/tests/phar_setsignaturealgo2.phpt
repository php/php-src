--TEST--
Phar::setSupportedSignatures() with hash
--EXTENSIONS--
phar
openssl
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
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$p = new Phar($fname);
$p['file1.txt'] = 'hi';

echo "Default:\n";
var_dump($p->getSignature());

echo "Set MD5:\n";
$p->setSignatureAlgorithm(Phar::MD5);
var_dump($p->getSignature());

echo "Set SHA1:\n";
$p->setSignatureAlgorithm(Phar::SHA1);
var_dump($p->getSignature());

echo "Set SHA256:\n";
try {
    $p->setSignatureAlgorithm(Phar::SHA256);
    var_dump($p->getSignature());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Set SHA512:\n";
try {
    $p->setSignatureAlgorithm(Phar::SHA512);
    var_dump($p->getSignature());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Set OPENSSL:\n";
try {
    $config = __DIR__ . '/files/openssl.cnf';
    $config_arg = array('config' => $config);
    $private = openssl_get_privatekey(file_get_contents(__DIR__ . '/files/private.pem'));
    $pkey = '';
    openssl_pkey_export($private, $pkey, NULL, $config_arg);
    $p->setSignatureAlgorithm(Phar::OPENSSL, $pkey);
    var_dump($p->getSignature());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar');
?>
--EXPECTF--
Default:
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(7) "SHA-256"
}
Set MD5:
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(3) "MD5"
}
Set SHA1:
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(5) "SHA-1"
}
Set SHA256:
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(7) "SHA-256"
}
Set SHA512:
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(7) "SHA-512"
}
Set OPENSSL:
array(2) {
  ["hash"]=>
  string(%d) "%s"
  ["hash_type"]=>
  string(7) "OpenSSL"
}

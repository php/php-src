--TEST--
Phar::setSignatureAlgorithm() with native OpenSSL and without ext/openssl
--EXTENSIONS--
phar
--SKIPIF--
<?php
if (extension_loaded("openssl")) die("skip ext/openssl must be disabled for this test");
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

$pkey = '-----BEGIN PRIVATE KEY-----
MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAMDcANSIpkgSF6Rh
KHM8JncsVuCsO5XjiMf3g50lB+poJAG9leoygbVtY55h9tzeI7SAdZbdIoHbtJ/V
kGdzlzX5jMGbH1sWKk5fZbai4pLZigd4ihH2V4M27jKrAGy6CAU8ZU/Ez2KQQj5g
A4ZVMJ3iZXlqCmRWwcs0lZvP+c9XAgMBAAECgYAaJLioFu4TjwBNdC47kMfWF9if
FDnvk6yTDuZ0gvSTvhJDeiO8X6Rdp7p9WeJRBnvomBFYphlraREPKbAtlenFVuIY
v10O9BjxkQ0O1Y7L2ztMO3E2LFtmWgoGimAnsbUHTkuB61Hd2AWdA7C357eQ67vZ
GlLu2HIFpSbzMcJFIQJBAPD6Hm7ETuL0ILwofImXAahHbwpmCtKmjvjJaFD5vWXP
FD6uTbBOgUP+n5Y17+d/vxhSX9yrQueAIodju3bbxUsCQQDM4fMCO4OUYbMroql7
ruIqBd34akrA+v2JoV+bMAE6RHBC6DgsI3uySbMJfmnPGoxlbXE0gKN4ONawwDd3
gTKlAkEAnJc8DWidhpdzajG488Pf/NUmkBBNOiOnxn1Cv1P6Ql01X6HutAHfuCqO
05KLKdj2ebyVtJTJrhuy1F33pL4dTwJBAKnIEB3ofahnshdV64cALJFQXVpvktUK
6TG1Vcn/ZPUJI9J+J5aELQxYwJH8fOhQAspGgEpW06Bb0aWVFCHnIbUCQBFVhu+P
RcHLpdSl7lZmws1bCnDUmt5GzKBw9diHxuyfGEJ0c0clDTWVEMyO80u0jxrliMkT
8h5bvpPaY8KIlkg=
-----END PRIVATE KEY-----';

try {
    $p->setSignatureAlgorithm(Phar::OPENSSL, $pkey);
    var_dump($p->getSignature());
} catch (Exception $e) {
    echo $e->getMessage();
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar');
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

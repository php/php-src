--TEST--
openssl_sign() polyfill with wrong return value
--EXTENSIONS--
phar
zend_test
--SKIPIF--
<?php
if (getenv('SKIP_SLOW_TESTS')) die('skip');
if (function_exists('openssl_sign')) die('skip requires openssl disabled for mocking purposes');
?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.tar';

function openssl_sign() {
    return zend_return_non_interned_string('foobar');
}

$phar = new PharData($fname);
$phar->setSignatureAlgorithm(Phar::OPENSSL, "randomcrap");
try {
    $phar->addEmptyDir('blah');
} catch (PharException $e) {
    echo $e->getMessage();
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar');
?>
--EXPECTF--
phar error: unable to write signature to tar-based phar: unable to write phar "%s" with requested openssl signature

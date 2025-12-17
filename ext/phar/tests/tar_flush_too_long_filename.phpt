--TEST--
Tar flush with too long file name
--EXTENSIONS--
phar
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

// Mock sign to fail at second invocation, tricks failure in phar_create_signature()
function openssl_sign() {
    static $counter = 0;
    $counter++;
    if ($counter === 2) {
        return false;
    }
    return true;
}

$phar = new PharData($fname);
$phar->addEmptyDir('blah1/');
$phar->setSignatureAlgorithm(Phar::OPENSSL, "randomcrap");
try {
    $phar->addEmptyDir('blah2/' . str_repeat('X', 1000));
} catch (PharException $e) {
    echo $e->getMessage();
}

?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar');
?>
--EXPECTF--
tar-based phar "%s" cannot be created, filename "%s" is too long for tar file format

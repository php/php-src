--TEST--
Test that the private key parameter of Phar::setSignatureAlgorithm() is marked sensitive.
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.tar';
$p = new PharData($fname);
$p['file1.txt'] = 'hi';

try {
    $p->setSignatureAlgorithm(-1, 'secret-private-key');
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar');
?>
--EXPECTF--
UnexpectedValueException: Unknown signature algorithm specified in %s:%d
Stack trace:
#0 %s(%d): PharData->setSignatureAlgorithm(-1, Object(SensitiveParameterValue))
#1 {main}

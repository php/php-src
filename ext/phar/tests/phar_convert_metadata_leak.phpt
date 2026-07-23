--TEST--
Phar convert logic leaks metadata
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.zip';

$phar = new PharData($fname);
$phar->setMetadata("foobar");
$phar['x'] = 'hi';
try {
    $phar->convertToData(Phar::ZIP, Phar::NONE, 'phar.zip');
} catch (BadMethodCallException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.zip');?>
--EXPECTF--
BadMethodCallException: data phar "%s" has invalid extension phar.zip

--TEST--
Phar: truncated tar
--EXTENSIONS--
phar
--FILE--
<?php
try {
    $p = new PharData(__DIR__ . '/files/trunc.tar');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
UnexpectedValueException: phar error: "%strunc.tar" is a corrupted tar file (truncated)

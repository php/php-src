--TEST--
GH-19752 (Phar decompression with invalid extension can cause UAF)
--EXTENSIONS--
phar
--FILE--
<?php
$phar = new PharData(__DIR__.'/gh19752.1');
try {
    $phar->decompress("*");
} catch (BadMethodCallException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
data phar converted from "%sgh19752.1" has invalid extension *

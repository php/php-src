--TEST--
Bug #77396 Relative filename exceeding maximum path length causes null pointer dereference.
--EXTENSIONS--
phar
--FILE--
<?php
$path = '../' . str_repeat("x", PHP_MAXPATHLEN) . '.tar';
try {
    $phar = new PharData($path);
} catch (UnexpectedValueException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Phar creation or opening failed

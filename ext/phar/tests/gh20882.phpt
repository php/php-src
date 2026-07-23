--TEST--
GH-20882 (phar buildFromIterator breaks with missing base directory)
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$phar = new \Phar(__DIR__ . "/test.phar");
try {
    $phar->buildFromIterator(
        new RecursiveIteratorIterator(
            new RecursiveDirectoryIterator(__DIR__.'/test79082', FilesystemIterator::SKIP_DOTS)
        ),
        null
    );
} catch (BadMethodCallException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
BadMethodCallException: Iterator RecursiveIteratorIterator returns an SplFileInfo object, so base directory must be specified

--TEST--
Phar: tar with relative link to subdirectory file from subdirectory file
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php
try {
    $p = new PharData(__DIR__ . '/files/subdirlink.tar');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
echo $p['hi/test.txt']->getContent();
echo $p['hi/link.txt']->getContent();
?>
--EXPECT--
hi
hi

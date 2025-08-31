--TEST--
Phar: tar with link to root directory file from root directory file
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php
try {
    $p = new PharData(__DIR__ . '/files/tinylink.tar');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
echo $p['file.txt']->getContent();
echo $p['link.txt']->getContent();
?>
--EXPECT--
hi
hi

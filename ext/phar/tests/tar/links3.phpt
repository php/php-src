--TEST--
Phar: tar with link to absolute path
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php
try {
    $p = new PharData(__DIR__ . '/files/biglink.tar');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
echo $p['file.txt']->getContent();
echo $p['my/file']->getContent();
?>
--EXPECT--
my file
my file

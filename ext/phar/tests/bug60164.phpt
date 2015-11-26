--TEST--
Phar: verify stub of specific length does not break __HALT_COMPILER(); scanning in php
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$phar = __DIR__ . '/files/stuboflength1041.phar';
foreach (new RecursiveIteratorIterator(new Phar($phar, null, 'stuboflength1041.phar')) as $item) {
    var_dump($item->getFileName());
}
?>
===DONE===
--EXPECT--
string(5) "a.php"
string(5) "b.php"
===DONE===
--TEST--
phar decompression: compressed entry round-trips correctly
--EXTENSIONS--
phar
zlib
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$pname = 'phar://' . $fname;

$phar = new Phar($fname);
$phar['entry.txt'] = 'hello world';
$phar['entry.txt']->compress(Phar::GZ);

echo file_get_contents($pname . '/entry.txt') . "\n";
echo "no crash";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar');
?>
--EXPECT--
hello world
no crash

--TEST--
Phar::setAlias() zip-based
--EXTENSIONS--
phar
zlib
bz2
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.zip';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.2.phar.zip';
$fname3 = __DIR__ . '/' . basename(__FILE__, '.php') . '.3.phar.zip';

$phar = new Phar($fname);
$phar->setStub('<?php echo "first stub\n"; __HALT_COMPILER(); ?>');
$phar->setAlias('hio');

$files = array();

$files['a'] = 'a';
$files['b'] = 'b';
$files['c'] = 'c';

foreach ($files as $n => $file) {
    $phar[$n] = $file;
}
$phar->stopBuffering();

echo $phar->getAlias() . "\n";
$phar->setAlias('test');
echo $phar->getAlias() . "\n";

// test compression

$phar->compressFiles(Phar::GZ);
copy($fname, $fname2);
$phar->setAlias('unused');
$p2 = new Phar($fname2);
echo $p2->getAlias(), "\n";
$p2->compressFiles(Phar::BZ2);
copy($fname2, $fname3);
$p2->setAlias('unused2');
$p3 = new Phar($fname3);
echo $p3->getAlias(), "\n";
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.2.phar.zip');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.3.phar.zip');
?>
--EXPECT--
hio
test
test
test

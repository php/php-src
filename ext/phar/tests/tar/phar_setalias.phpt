--TEST--
Phar::setAlias() tar-based
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.copy.phar.tar';

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

copy($fname, $fname2);
$phar->setAlias('unused');
$a = new Phar($fname2);
echo $a->getAlias() . "\n";

?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.copy.phar.tar');
?>
--EXPECT--
hio
test
test

--TEST--
Phar::setAlias() error zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.zip';

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
$b = $phar;
$phar = new Phar(__DIR__ . '/notphar.phar');
try {
    $phar->setAlias('test');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
__HALT_COMPILER();
?>
--EXPECTF--
hio
test
alias "test" is already used for archive "%sphar_setalias2.phar.zip" and cannot be used for other archives

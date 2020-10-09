--TEST--
Phar::chmod tar-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar';
$alias = 'phar://hio';

$phar = new Phar($fname);
$phar['a.php'] = '<?php echo "This is a\n"; include "'.$alias.'/b.php"; ?>';
$phar->setAlias('hio');
$phar->addEmptyDir('test');
$phar->stopBuffering();

try {
    var_dump($phar['a.php']->isExecutable());
    $phar['a.php']->chmod(0777);
    var_dump($phar['a.php']->isExecutable());
    $phar['a.php']->chmod(0666);
    var_dump($phar['a.php']->isExecutable());
    echo "test dir\n";
    var_dump($phar['test']->isReadable());
    $phar['test']->chmod(0000);
    var_dump($phar['test']->isReadable());
    $phar['test']->chmod(0666);
    var_dump($phar['test']->isReadable());
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
test dir
bool(true)
bool(false)
bool(true)

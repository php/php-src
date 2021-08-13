--TEST--
Phar::chmod zip-based
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.zip';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.2.phar.zip';
$alias = 'phar://hio';

$phar = new Phar($fname);
$phar['a.php'] = '<?php echo "This is a\n"; include "'.$alias.'/b.php"; ?>';
$phar->setAlias('hio');
$phar->addEmptyDir('test');
$phar->stopBuffering();

try {
    var_dump($phar['a.php']->isExecutable());
    $phar['a.php']->chmod(0777);
    copy($fname, $fname2);
    $phar->setAlias('unused');
    $phar2 = new Phar($fname2);
    var_dump($phar2['a.php']->isExecutable());
    $phar['a.php']->chmod(0666);
    var_dump($phar['a.php']->isExecutable());
    echo "test dir\n";
    var_dump($phar['test']->isDir());
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
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.2.phar.zip');
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
test dir
bool(true)
bool(true)
bool(false)
bool(true)

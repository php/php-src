--TEST--
Phar: Phar::setDefaultStub() with and without arg, zip-based phar
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.zip';

$phar = new Phar($fname);
$phar['a.php'] = '<php echo "this is a\n"; ?>';
$phar['b.php'] = '<php echo "this is b\n"; ?>';
$phar->setStub('<?php echo "Hello World\n"; __HALT_COMPILER(); ?>');

var_dump($phar->getStub());

echo "============================================================================\n";
echo "============================================================================\n";

try {
    $phar->setDefaultStub();
    $phar->stopBuffering();
} catch(Exception $e) {
    echo $e->getMessage(). "\n";
}

var_dump($phar->getStub());

echo "============================================================================\n";
echo "============================================================================\n";

try {
    $phar->setDefaultStub('my/custom/thingy.php');
} catch(Error $e) {
    echo $e->getMessage(). "\n";
}

try {
    $phar->stopBuffering();
} catch(Exception $e) {
    echo $e->getMessage(). "\n";
}

var_dump($phar->getStub());

echo "============================================================================\n";
echo "============================================================================\n";

try {
    $phar->setDefaultStub('my/custom/thingy.php', 'the/web.php');
} catch(ValueError $e) {
    echo $e->getMessage(). "\n";
}

try {
    $phar->stopBuffering();
} catch(Exception $e) {
    echo $e->getMessage(). "\n";
}

var_dump($phar->getStub());

?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
?>
--EXPECT--
string(51) "<?php echo "Hello World\n"; __HALT_COMPILER(); ?>
"
============================================================================
============================================================================
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
============================================================================
============================================================================
Phar::setDefaultStub(): Argument #1 ($index) must be null for a tar- or zip-based phar stub, string given
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
============================================================================
============================================================================
Phar::setDefaultStub(): Argument #1 ($index) must be null for a tar- or zip-based phar stub, string given
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"

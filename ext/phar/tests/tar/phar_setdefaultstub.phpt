--TEST--
Phar: Phar::setDefaultStub() with and without arg, tar-based phar
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar';

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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

var_dump($phar->getStub());

echo "============================================================================\n";
echo "============================================================================\n";

try {
    $phar->setDefaultStub('my/custom/thingy.php');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
    $phar->stopBuffering();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
var_dump($phar->getStub());

echo "============================================================================\n";
echo "============================================================================\n";


try {
    $phar->setDefaultStub('my/custom/thingy.php', 'the/web.php');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
    $phar->stopBuffering();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

var_dump($phar->getStub());

?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
?>
--EXPECT--
string(51) "<?php echo "Hello World\n"; __HALT_COMPILER(); ?>
"
============================================================================
============================================================================
string(60) "<?php // tar-based phar archive stub file
__HALT_COMPILER();"
============================================================================
============================================================================
ValueError: Phar::setDefaultStub(): Argument #1 ($index) must be null for a tar- or zip-based phar stub, string given
string(60) "<?php // tar-based phar archive stub file
__HALT_COMPILER();"
============================================================================
============================================================================
ValueError: Phar::setDefaultStub(): Argument #1 ($index) must be null for a tar- or zip-based phar stub, string given
string(60) "<?php // tar-based phar archive stub file
__HALT_COMPILER();"

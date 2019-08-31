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
	$phar->stopBuffering();
} catch(Exception $e) {
	echo $e->getMessage(). "\n";
}

var_dump($phar->getStub());

echo "============================================================================\n";
echo "============================================================================\n";

try {
	$phar->setDefaultStub('my/custom/thingy.php', 'the/web.php');
	$phar->stopBuffering();
} catch(Exception $e) {
	echo $e->getMessage(). "\n";
}

var_dump($phar->getStub());

?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
?>
--EXPECTF--
string(51) "<?php echo "Hello World\n"; __HALT_COMPILER(); ?>
"
============================================================================
============================================================================
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
============================================================================
============================================================================

Warning: Phar::setDefaultStub(): method accepts no arguments for a tar- or zip-based phar stub, 1 given in %sphar_setdefaultstub.php on line %d
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
============================================================================
============================================================================

Warning: Phar::setDefaultStub(): method accepts no arguments for a tar- or zip-based phar stub, 2 given in %sphar_setdefaultstub.php on line %d
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
===DONE===

--TEST--
Phar: Phar::setDefaultStub() with and without arg, zip-based phar
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip';

$phar = new Phar($fname);
$phar['a.php'] = '<php echo "this is a\n"; ?>';
$phar['b.php'] = '<php echo "this is b\n"; ?>';

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
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
?>
--EXPECT--
Stub cannot be changed in a zip-based phar
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
============================================================================
============================================================================
Stub cannot be changed in a zip-based phar
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
============================================================================
============================================================================
Stub cannot be changed in a zip-based phar
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
===DONE===

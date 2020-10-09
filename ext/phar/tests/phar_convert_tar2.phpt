--TEST--
Phar::convertToTar() gzip compressed
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar.gz';
$fname3 = __DIR__ . '/' . basename(__FILE__, '.php') . '.2.phar.tar';
$stub = '<?php echo "first stub\n"; __HALT_COMPILER(); ?>';
$file = $stub;

$files = array();
$files['a'] = 'a';
$files['b'] = 'b';
$files['c'] = 'c';

include 'files/phar_test.inc';

$phar = new Phar($fname);
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->isCompressed());
var_dump($phar->getStub());

$phar = $phar->convertToExecutable(Phar::TAR, Phar::GZ);
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->isCompressed());
var_dump($phar->getStub());

copy($fname2, $fname3);

$phar = new Phar($fname3);
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->isCompressed() == Phar::GZ);
var_dump($phar->getStub());

?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar.gz');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.2.phar.tar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
?>
--EXPECT--
bool(false)
bool(false)
string(48) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>"
bool(true)
int(4096)
string(60) "<?php // tar-based phar archive stub file
__HALT_COMPILER();"
bool(true)
bool(true)
string(60) "<?php // tar-based phar archive stub file
__HALT_COMPILER();"

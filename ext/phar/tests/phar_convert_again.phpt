--TEST--
Phar::conversion to other formats
--EXTENSIONS--
phar
zlib
bz2
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '2.tbz';
$pname = 'phar://' . $fname;
$stub = '<?php echo "first stub\n"; __HALT_COMPILER(); ?>';
$file = $stub;

$files = array();
$files['a'] = 'a';
$files['b'] = 'b';
$files['c'] = 'c';

include 'files/phar_test.inc';

$phar = new Phar($fname);
$zip = $phar->convertToData(Phar::ZIP);
echo $zip->getPath() . "\n";
$tgz = $phar->convertToData(Phar::TAR, Phar::GZ);
echo $tgz->getPath() . "\n";
$tbz = $phar->convertToData(Phar::TAR, Phar::BZ2);
echo $tbz->getPath() . "\n";
try {
	$phar = $tbz->convertToExecutable(Phar::PHAR, Phar::NONE);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
copy($tbz->getPath(), $fname2);
$tbz = new PharData($fname2);
$phar = $tbz->convertToExecutable(Phar::PHAR, Phar::NONE);
echo $phar->getPath() . "\n";
$phar['a'] = 'hi';
$phar['a']->setMetadata('hi');
$zip = $phar->convertToExecutable(Phar::ZIP);
echo $zip->getPath() . "\n";
echo $zip['a']->getMetadata() . "\n";
$data = $zip->convertToData();
echo $data->getPath() . "\n";
// extra code coverage
try {
	$data->setStub('hi');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$data->setDefaultStub();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$data->setAlias('hi');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
$tar = $phar->convertToExecutable(Phar::TAR);
echo $tar->getPath() . "\n";
$data = $tar->convertToData();
echo $data->getPath() . "\n";
$tgz = $tar->convertToExecutable(null, Phar::GZ);
echo $tgz->getPath() . "\n";
try {
	$tgz->convertToExecutable(25);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$tgz->convertToExecutable(Phar::ZIP, Phar::GZ);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$tgz->convertToExecutable(Phar::ZIP, Phar::BZ2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$phar->convertToData();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$tgz->convertToData(Phar::PHAR);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$tgz->convertToData(25);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$tgz->convertToData(Phar::ZIP, Phar::GZ);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$tgz->convertToData(Phar::ZIP, Phar::BZ2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$tgz->convertToExecutable(Phar::TAR, 25);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$tgz->convertToData(Phar::TAR, 25);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
// extra code coverage
try {
	$data->setStub('hi');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$data->setAlias('hi');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$data->setDefaultStub();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$tgz->convertToData(Phar::TAR, Phar::GZ, '.phar.tgz.oops');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
	$phar->convertToExecutable(Phar::TAR, Phar::GZ, '.tgz.oops');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
	$tgz->convertToData(Phar::TAR, Phar::GZ, '.phar/.tgz.oops');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.zip');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar.gz');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar.bz2');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '2.tbz');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '2.phar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '2.phar.tar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '2.tar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '2.phar.zip');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '2.zip');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '2.phar.tar.gz');
__HALT_COMPILER();
?>
--EXPECTF--
%sphar_convert_again.zip
%sphar_convert_again.tar.gz
%sphar_convert_again.tar.bz2
BadMethodCallException: Unable to add newly converted phar "%sphar_convert_again.phar" to the list of phars, a phar with that name already exists
%sphar_convert_again2.phar
%sphar_convert_again2.phar.zip
hi
%sphar_convert_again2.zip
UnexpectedValueException: A Phar stub cannot be set in a plain zip archive
UnexpectedValueException: A Phar stub cannot be set in a plain zip archive
A Phar alias cannot be set in a plain zip archive
%sphar_convert_again2.phar.tar
%sphar_convert_again2.tar
%sphar_convert_again2.phar.tar.gz
ValueError: Phar::convertToExecutable(): Argument #1 ($format) must be one of Phar::PHAR, Phar::TAR, or Phar::ZIP
ValueError: Phar::convertToExecutable(): Argument #2 ($compression) must not be Phar::GZ when argument #1 is Phar::ZIP, as ZIP archives do not support whole-archive compression
ValueError: Phar::convertToExecutable(): Argument #2 ($compression) must not be Phar::BZ2 when argument #1 is Phar::ZIP, as ZIP archives do not support whole-archive compression
UnexpectedValueException: Cannot write out data phar archive, use Phar::TAR or Phar::ZIP
UnexpectedValueException: Cannot write out data phar archive, use Phar::TAR or Phar::ZIP
ValueError: Phar::convertToData(): Argument #1 ($format) must be one of Phar::TAR or Phar::ZIP
ValueError: Phar::convertToData(): Argument #2 ($compression) must not be Phar::GZ when argument #1 is Phar::ZIP, as ZIP archives do not support whole-archive compression
ValueError: Phar::convertToData(): Argument #2 ($compression) must not be Phar::BZ2 when argument #1 is Phar::ZIP, as ZIP archives do not support whole-archive compression
ValueError: Phar::convertToExecutable(): Argument #2 ($compression) must be one of Phar::NONE, Phar::GZ, or Phar::BZ2
ValueError: Phar::convertToData(): Argument #2 ($compression) must be one of Phar::NONE, Phar::GZ, or Phar::BZ2
UnexpectedValueException: A Phar stub cannot be set in a plain tar archive
UnexpectedValueException: A Phar alias cannot be set in a plain tar archive
UnexpectedValueException: A Phar stub cannot be set in a plain tar archive
BadMethodCallException: data phar "%sphar_convert_again2.phar.tgz.oops" has invalid extension phar.tgz.oops
BadMethodCallException: phar "%sphar_convert_again2.tgz.oops" has invalid extension tgz.oops
BadMethodCallException: data phar "%sphar_convert_again2.phar/.tgz.oops" has invalid extension phar/.tgz.oops

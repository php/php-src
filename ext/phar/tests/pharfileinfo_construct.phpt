--TEST--
Phar: PharFileInfo::__construct
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$pname = 'phar://' . $fname;

try {
	file_put_contents($fname, 'blah');
	$a = new PharFileInfo($pname . '/oops');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
	unlink($fname);
}

try {
	$a = new PharFileInfo(array());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

$a = new Phar($fname);
$a['a'] = 'hi';
$b = $a['a'];

try {
	$a = new PharFileInfo($pname . '/oops/I/do/not/exist');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
	$b->__construct('oops');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
	$a = new PharFileInfo(__FILE__);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
--EXPECTF--

RuntimeException: Cannot open phar file 'phar://%spharfileinfo_construct.phar/oops': internal corruption of phar "%spharfileinfo_construct.phar" (truncated entry)
TypeError: PharFileInfo::__construct(): Argument #1 ($filename) must be of type string, array given
RuntimeException: Cannot access phar file entry '/oops/I/do/not/exist' in archive '%s'
Error: Cannot call constructor twice
RuntimeException: '%s' is not a valid phar archive URL (must have at least phar://filename.phar)

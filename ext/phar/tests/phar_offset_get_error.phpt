--TEST--
Phar: ignore filenames starting with / on offsetSet
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://'.$fname;
$iname = '/file.txt';
$ename = '/error/..';

$p = new Phar($fname);
$p[$iname] = "foobar\n";

try {
    $p[$ename] = "foobar\n";
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

include($pname . $iname);

// extra coverage
try {
	$p['.phar/oops'] = 'hi';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$a = $p['.phar/stub.php'];
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
BadMethodCallException: Entry /error/.. does not exist and cannot be created: phar error: invalid path "/error/.." contains upper directory reference
foobar
ValueError: Phar::offsetSet(): Argument #1 ($localName) must not start with ".phar"
BadMethodCallException: Entry .phar/stub.php does not exist
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

try
{
    $p[$ename] = "foobar\n";
}
catch(Exception $e)
{
    echo $e->getMessage() . "\n";
}

include($pname . $iname);

// extra coverage
try {
$p['.phar/oops'] = 'hi';
} catch (Exception $e) {
echo $e->getMessage(),"\n";
}
try {
$a = $p['.phar/stub.php'];
} catch (Exception $e) {
echo $e->getMessage(),"\n";
}
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
Entry /error/.. does not exist and cannot be created: phar error: invalid path "/error/.." contains upper directory reference
foobar
Cannot set any files or directories in magic ".phar" directory
Entry .phar/stub.php does not exist

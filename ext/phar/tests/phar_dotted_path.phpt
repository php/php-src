--TEST--
Phar: create new Phar with broken.dirname in path
--EXTENSIONS--
phar
--INI--
default_charset=
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$dir = __DIR__ . '/broken.dirname';
mkdir($dir, 0777);

$fname = $dir . '/dotted_path.phar';
$stub = Phar::createDefaultStub();
$file = $stub;

$files = array();
$files['a'] = 'this is a';
$files['b'] = 'this is b';

include 'files/phar_test.inc';

$phar = new Phar($fname);

foreach ($phar as $entry) {
    echo file_get_contents($entry)."\n";
}

?>
--CLEAN--
<?php
unlink(__DIR__ . '/broken.dirname/dotted_path.phar');
rmdir(__DIR__ . '/broken.dirname');
?>
--EXPECT--
this is a
this is b

--TEST--
FileInfo - Calling the constructor twice
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

$x = new finfo;
$x->finfo();

echo "done!\n";

?>
--EXPECT--
done!

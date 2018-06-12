--TEST--
FileInfo - Calling the constructor twice
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php

$x = new finfo;
$x->finfo();

echo "done!\n";

?>
--EXPECT--
done!



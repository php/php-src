--TEST--
FileInfo - Calling the constructor twice
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

$x = new finfo;
$x->finfo();

echo "done!\n";

?>
--EXPECTF--
done!



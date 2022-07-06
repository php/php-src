--TEST--
Incorrect CFG/SSA reconstruction
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
if (!is_int($info = gc_collect_cycles()) || ($info < 100)) {
        echo gettype($info)."\n";
}
?>
--EXPECT--
integer

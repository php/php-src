--TEST--
mb_split()
--SKIPIF--
<?php include('skipif.inc'); ?>
function_exists('mb_split') or die("SKIP");
--POST--
--GET--
--FILE--
<?php include('022.inc'); ?>
--EXPECT--
bool(true)
ok
ok
ok
ok
ok
ok
2-2
3-3
4-4

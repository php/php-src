--TEST--
mb_ereg_replace()
--SKIPIF--
<?php include('skipif.inc'); ?>
function_exists('mb_ereg_replace') or die("SKIP");
--POST--
--GET--
--FILE--
<?php include('021.inc'); ?>
--EXPECT--
a-b-c-d-e
[abc] [def] [ghi]


--TEST--
Bug #24281 (str_replace count not returned if variable wasn't initialized)
--SKIPIF--
<?php 
        if (version_compare(zend_version(), '2.0.0-dev', '<')) die('SKIP PHP5 functionality'); 
?>
--FILE--
<?php
$string = "He had had to have had it";
$newstring = str_replace("had", "foo", $string, $count);
print "$count changes were made.\n";
$count = "foo";
$newstring = str_replace("had", "foo", $string, $count);
print "$count changes were made.\n";
?>
--EXPECT--
3 changes were made.
3 changes were made.

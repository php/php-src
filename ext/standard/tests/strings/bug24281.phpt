--TEST--
Bug #24281 (str_replace count not returned if variable wasn't initialized)
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

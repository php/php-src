--TEST--
filter() test
--POST--
foo=<b>abc</b>
--GET--
--FILE--
<?php 
echo input_get(INPUT_POST, 'foo', FS_STRIPPED);
?>
--EXPECT--
abc

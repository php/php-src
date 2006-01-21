--TEST--
filter() test
--POST--
foo=<b>abc</b>
--FILE--
<?php 
echo input_get(INPUT_POST, 'foo', FILTER_SANITIZE_STRIPPED);
?>
--EXPECT--
abc

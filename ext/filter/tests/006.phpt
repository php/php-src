--TEST--
filter() test
--POST--
foo=<b>abc</b>
--GET--
--FILE--
<?php 
echo filter(FILTER_POST, 'foo', FILTER_HTML);
?>
--EXPECT--
abc

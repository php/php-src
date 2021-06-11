--TEST--
filter_input() test
--EXTENSIONS--
filter
--POST--
foo=<b>abc</b>
--FILE--
<?php
echo filter_input(INPUT_POST, 'foo', FILTER_SANITIZE_STRIPPED);
?>
--EXPECT--
abc

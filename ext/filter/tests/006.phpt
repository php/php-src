--TEST--
filter() test
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--POST--
foo=<b>abc</b>
--FILE--
<?php
echo filter_input(INPUT_POST, 'foo', FILTER_SANITIZE_STRIPPED);
?>
--EXPECT--
abc

--TEST--
filter_input() test
--EXTENSIONS--
filter
--POST--
foo=<b>abc</b>
--FILE--
<?php
echo filter_input(INPUT_POST, 'foo', FILTER_SANITIZE_SPECIAL_CHARS);
?>
--EXPECT--
&#60;b&#62;abc&#60;/b&#62;

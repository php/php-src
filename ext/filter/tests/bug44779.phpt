--TEST--
Bug #44779 (filter returns NULL in CLI when it shouldn't)
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_input(INPUT_SERVER, "PHP_SELF"));
?>
--EXPECTF--
string(%d) "%s"

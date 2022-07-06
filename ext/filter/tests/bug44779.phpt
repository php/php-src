--TEST--
Bug #44779 (filter returns NULL in CLI when it shouldn't)
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
var_dump(filter_input(INPUT_SERVER, "PHP_SELF"));
?>
--EXPECTF--
string(%d) "%s"

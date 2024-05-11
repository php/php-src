--TEST--
mb_get_info("http_input") can return null
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(mb_get_info("http_input"));
?>
--EXPECT--
NULL

--TEST--
Bug #20934 (html_entity_decode() crash when "" is passed)
--FILE--
<?php
var_dump(html_entity_decode(""));
?>
--EXPECT--
string(0) ""

--TEST--
Bug #25707 (html_entity_decode over-decodes &amp;lt;)
--FILE--
<?php
var_dump(html_entity_decode("&amp;lt;", ENT_COMPAT, 'ISO-8859-1'));
?>
--EXPECT--
string(4) "&lt;"

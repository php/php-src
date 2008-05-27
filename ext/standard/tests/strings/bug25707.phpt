--TEST--
Bug #25707 (html_entity_decode over-decodes &amp;lt;)
--FILE--
<?php
var_dump(html_entity_decode("&amp;lt;", ENT_COMPAT, 'ISO-8859-1'));
var_dump(html_entity_decode("&amp;#38;", ENT_COMPAT, 'ISO-8859-1'));
var_dump(html_entity_decode("&amp;#38;lt;", ENT_COMPAT, 'ISO-8859-1'));
?>
--EXPECT--
unicode(4) "&lt;"
unicode(5) "&#38;"
unicode(8) "&#38;lt;"

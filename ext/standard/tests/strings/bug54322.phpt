--TEST--
Bug #54322: Null pointer deref in get_html_translation_table due to information loss in long-to-int conversion
--FILE--
<?php
var_dump(
get_html_translation_table(NAN, 0, "UTF-8") > 0
);
--EXPECTF--
Warning: get_html_translation_table() expects parameter 1 to be integer, float given in %s on line %d
bool(false)

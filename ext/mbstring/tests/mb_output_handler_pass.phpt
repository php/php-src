--TEST--
mb_output_handler() with output_encoding=pass
--EXTENSIONS--
mbstring
--INI--
output_handler=mb_output_handler
output_encoding=pass
--FILE--
<?php
var_dump(mb_http_output());
var_dump("\xff");
ob_end_flush();

ob_start('mb_output_handler');
mb_http_output("UTF-8");
var_dump("\xff");
ob_end_flush();

ob_start('mb_output_handler');
mb_http_output("pass");
var_dump("\xff");
?>
--EXPECTF--

Deprecated: Function mb_http_output() is deprecated since 8.5, use output_encoding INI settings instead in %s on line %d
string(4) "pass"
string(1) "ÿ"

Deprecated: Function mb_http_output() is deprecated since 8.5, use output_encoding INI settings instead in %s on line %d
string(1) "?"

Deprecated: Function mb_http_output() is deprecated since 8.5, use output_encoding INI settings instead in %s on line %d
string(1) "ÿ"

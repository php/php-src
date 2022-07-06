--TEST--
mb_output_handler() with output_encoding=pass
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
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
--EXPECT--
string(4) "pass"
string(1) "ÿ"
string(1) "?"
string(1) "ÿ"

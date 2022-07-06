--TEST--
mb_output_handler() and mbstring.http_output_conv_mimetypes (9)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
mbstring.http_output_conv_mimetypes=html
--FILE--
<?php
mb_http_output("EUC-JP");
header("Content-Type: application/xhtml+xml");
ob_start();
ob_start('mb_output_handler');
echo "テスト";
ob_end_flush();
var_dump(bin2hex(ob_get_clean()));
?>
--EXPECT--
string(12) "a5c6a5b9a5c8"

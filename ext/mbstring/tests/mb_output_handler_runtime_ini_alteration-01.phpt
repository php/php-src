--TEST--
mb_output_handler() and mbstring.http_output_conv_mimetypes alteration in runtime (1)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
mbstring.internal_encoding=UTF-8
mbstring.http_output_conv_mimetypes=plain
--FILE--
<?php
mb_http_output("EUC-JP");
ini_set('mbstring.http_output_conv_mimetypes', 'text');
header("Content-Type: text/html");
ob_start();
ob_start('mb_output_handler');
echo "テスト";
ob_end_flush();
var_dump(bin2hex(ob_get_clean()));
?>
--EXPECT--
string(12) "a5c6a5b9a5c8"

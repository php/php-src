--TEST--
mb_output_handler() and mbstring.http_output_conv_mimetypes (4)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
mbstring.internal_encoding=UTF-8
--FILE--
<?php
mb_http_output("EUC-JP");
header("Content-Type: application/octet-stream");
ob_start();
ob_start('mb_output_handler');
echo "テスト";
ob_end_flush();
var_dump(bin2hex(ob_get_clean()));
?>
--EXPECT--
string(18) "e38386e382b9e38388"

--TEST--
mb_output_handler() and mbstring.http_output_conv_mimetypes (10)
--EXTENSIONS--
mbstring
--INI--
mbstring.http_output_conv_mimetypes=html
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

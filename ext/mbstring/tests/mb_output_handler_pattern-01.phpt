--TEST--
mb_output_handler() and mbstring.http_output_conv_mimetypes (1)
--EXTENSIONS--
mbstring
--FILE--
<?php
mb_http_output("EUC-JP");
header("Content-Type: text/html");
ob_start();
ob_start('mb_output_handler');
echo "テスト";
ob_end_flush();
var_dump(bin2hex(ob_get_clean()));
?>
--EXPECT--
string(12) "a5c6a5b9a5c8"

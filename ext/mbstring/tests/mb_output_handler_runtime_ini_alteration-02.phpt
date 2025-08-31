--TEST--
mb_output_handler() and mbstring.http_output_conv_mimetypes alteration in runtime (2)
--EXTENSIONS--
mbstring
--INI--
internal_encoding=UTF-8
mbstring.http_output_conv_mimetypes=html
--FILE--
<?php
mb_http_output("EUC-JP");
ini_set('mbstring.http_output_conv_mimetypes', 'application');
header("Content-Type: text/html");
ob_start();
ob_start('mb_output_handler');
echo "テスト";
ob_end_flush();
var_dump(bin2hex(ob_get_clean()));
?>
--EXPECT--
string(18) "e38386e382b9e38388"

--TEST--
mb_output_handler() and mbstring.http_output_conv_mimetypes (5)
--EXTENSIONS--
mbstring
--INI--
output_encoding=EUC-JP
--FILE--
<?php
ob_start();
ob_start('mb_output_handler');
echo "テスト";
ob_end_flush();
var_dump(bin2hex(ob_get_clean()));
?>
--EXPECT--
string(12) "a5c6a5b9a5c8"

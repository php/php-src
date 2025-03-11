--TEST--
GH-17989 (mb_output_handler crash with unset http_output_conv_mimetypes)
--EXTENSIONS--
mbstring
--INI--
mbstring.http_output_conv_mimetypes=
--FILE--
<?php
echo "set mime type via this echo\n";
ob_start('mb_output_handler');
echo "hi";
ob_flush();
?>
--EXPECT--
set mime type via this echo
hi

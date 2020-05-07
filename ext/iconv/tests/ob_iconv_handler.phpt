--TEST--
ob_iconv_handler()
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
error_reporting=E_ALL
--FILE--
<?php
ini_set('internal_encoding', 'EUC-JP');
ini_set('output_encoding', 'Shift_JIS');
ob_start('ob_iconv_handler');
print "¤¢¤¤¤¦¤¨¤ª";
ob_end_flush();
?>
--EXPECT--
‚ ‚¢‚¤‚¦‚¨

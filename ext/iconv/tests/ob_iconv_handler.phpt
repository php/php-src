--TEST--
ob_output_handler
--SKIPIF--
<?php /* include('skipif.inc'); */ ?>
--INI--
error_reporting=2039
--FILE--
<?php
/* include('test.inc'); */
iconv_set_encoding('internal_encoding', 'EUC-JP');
iconv_set_encoding('output_encoding', 'Shift_JIS');
ob_start('ob_iconv_handler');
print "¤¢¤¤¤¦¤¨¤ª";
ob_end_flush();
?>
--EXPECT--
‚ ‚¢‚¤‚¦‚¨

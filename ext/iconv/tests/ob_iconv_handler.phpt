--TEST--
ob_iconv_handler()
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
error_reporting=2039
--FILE--
<?php
iconv_set_encoding('internal_encoding', 'EUC-JP');
iconv_set_encoding('output_encoding', 'Shift_JIS');
ob_start('ob_iconv_handler');
print "����������";
ob_end_flush();
?>
--EXPECT--
����������

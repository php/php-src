--TEST--
mb_http_input() 
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
a=日本語0123456789日本語カタカナひらがな
--GET--
b=日本語0123456789日本語カタカナひらがな
--FILE--
<?php include('003.inc'); ?>
--EXPECT--
日本語0123456789日本語カタカナひらがな
日本語0123456789日本語カタカナひらがな
OK


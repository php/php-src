--TEST--
mb_strcut()
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('013.inc'); ?>
--EXPECT--
の文
0123この文字列は日本語です。EUC-JPを使っています。日本語は面倒臭い。
OK
OK: 0123この文



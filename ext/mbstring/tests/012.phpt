--TEST--
mb_substr()
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('012.inc'); ?>
--EXPECT--
1: 日本語です。EUC-
2: 0123この文字列は日本語です。EUC-JPを使っています。日本語は面倒臭い。
3 OK
4 OK: 0123この文字列は



--TEST--
mb_strimwidth()
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('014.inc'); ?>
--EXPECT--
1: 0123この文字...
2: 0123この文字列は日本語です。EUC-JPを使っています。日本語は面倒臭い。
3: 。EUC-JPを使っています。日本語は面倒臭い。
ERR: Warning
4 OK
ERR: Warning
5 OK
ERR: Warning
6 OK



--TEST--
mb_output_handler() (EUC-JP)
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('007.inc'); ?>
--EXPECT--
string(73) "テスト用日本語文字列。このモジュールはPHPにマルチバイト関数を提供します。"

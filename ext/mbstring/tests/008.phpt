--TEST--
mb_preferred_mime_name()
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('008.inc'); ?>
--EXPECT--
Shift_JIS
Shift_JIS
EUC-JP
UTF-8
ISO-2022-JP
ISO-2022-JP
ISO-8859-1
UCS-2
UCS-4
== INVALID PARAMETER ==
ERR: Warning
OK_BAD_NAME


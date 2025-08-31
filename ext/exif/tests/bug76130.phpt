--TEST--
Bug #76130 (Heap Buffer Overflow (READ: 1786) in exif_iif_add_value)
--DESCRIPTION--
This test is meant to exhibit memory issues with the `-m` option. Since a lot of
notices and warnings are to be expected anyway, we suppress these, since the are
not relevant for this test.
--INI--
error_reporting=E_ALL & ~E_WARNING & ~E_NOTICE
--EXTENSIONS--
exif
--FILE--
<?php
exif_read_data(__DIR__ . '/bug76130_1.jpg');
exif_read_data(__DIR__ . '/bug76130_2.jpg');
?>
===DONE===
--EXPECT--
===DONE===

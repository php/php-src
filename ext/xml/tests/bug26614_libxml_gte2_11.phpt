--TEST--
Bug #26614 (CDATA sections skipped on line count)
--EXTENSIONS--
xml
--SKIPIF--
<?php
require __DIR__ . '/libxml_expat_skipif.inc';
skipif(want_expat: false);
if (LIBXML_VERSION < 21100) die('skip libxml2 test variant for version >= 2.11');
?>
--FILE--
<?php
require __DIR__ . '/bug26614.inc';
?>
--EXPECTF--
CDATA
<DATA> at line 2, col %d (byte 50)
</DATA> at line 9, col %d (byte 96)
Comment
<DATA> at line 2, col %d (byte 50)
</DATA> at line 9, col %d (byte 96)
Text
<DATA> at line 2, col %d (byte 50)
</DATA> at line 9, col %d (byte 96)

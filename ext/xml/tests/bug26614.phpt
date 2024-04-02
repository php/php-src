--TEST--
Bug #26614 (CDATA sections skipped on line count)
--EXTENSIONS--
xml
--SKIPIF--
<?php
require __DIR__ . '/libxml_expat_skipif.inc';
skipif(want_expat: true);
?>
--FILE--
<?php
require __DIR__ . '/bug26614.inc';
?>
--EXPECT--
CDATA
<DATA> at line 2, col 0 (byte 45)
</DATA> at line 9, col 0 (byte 89)
Comment
<DATA> at line 2, col 0 (byte 45)
</DATA> at line 9, col 0 (byte 89)
Text
<DATA> at line 2, col 0 (byte 45)
</DATA> at line 9, col 0 (byte 89)

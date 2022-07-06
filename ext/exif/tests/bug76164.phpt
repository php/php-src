--TEST--
Bug #76164 (exif_read_data zend_mm_heap corrupted)
--SKIPIF--
<?php
if (!extension_loaded('exif')) die('skip exif extension not available');
?>
--FILE--
<?php
$var1 = 'nonexistentfile';
$var2 = 2200000000;
@exif_read_data($var1, $var2); // we're not interested in the warning, here
$var2 = 1;
?>
===DONE===
--EXPECT--
===DONE===

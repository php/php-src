--TEST--
Bug #71606 (Segmentation fault mb_strcut + mb_list_encodings)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip ext/mbstring not available');
?>
--FILE--
<?php
echo mb_strcut('&quot;', 0, 0, 'HTML-ENTITIES');
echo 'DONE', PHP_EOL;
?>
--EXPECT--
DONE

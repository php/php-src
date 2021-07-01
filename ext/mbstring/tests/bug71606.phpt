--TEST--
Bug #71606 (Segmentation fault mb_strcut + mb_list_encodings)
--EXTENSIONS--
mbstring
--FILE--
<?php
echo mb_strcut('&quot;', 0, 0, 'HTML-ENTITIES');
echo 'DONE', PHP_EOL;
?>
--EXPECT--
DONE

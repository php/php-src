--TEST--
Bug #71606 (Segmentation fault mb_strcut + mb_list_encodings)
--EXTENSIONS--
mbstring
--FILE--
<?php
echo mb_strcut('&quot;', 0, 0, 'HTML-ENTITIES');
echo 'DONE', PHP_EOL;
?>
--EXPECTF--
Deprecated: mb_strcut(): Handling HTML entities via mbstring is deprecated; use htmlspecialchars, htmlentities, or mb_encode_numericentity/mb_decode_numericentity instead in %s on line %d
DONE

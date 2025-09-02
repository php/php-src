--TEST--
Bug #45722 (mb_check_encoding() crashes)
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(mb_check_encoding("&\xc2\xb7 TEST TEST TEST TEST TEST TEST", "HTML-ENTITIES"));
?>
--EXPECTF--
Deprecated: mb_check_encoding(): Handling HTML entities via mbstring is deprecated; use htmlspecialchars, htmlentities, or mb_encode_numericentity/mb_decode_numericentity instead in %s on line %d
bool(true)

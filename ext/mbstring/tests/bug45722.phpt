--TEST--
Bug #45722 (mb_check_encoding() crashes)
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(mb_check_encoding("&\xc2\xb7 TEST TEST TEST TEST TEST TEST", "HTML-ENTITIES"));
?>
--EXPECT--
bool(true)

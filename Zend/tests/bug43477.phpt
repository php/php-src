--TEST--
Bug #43477 (Unicode error mode)
--FILE--
<?php
var_dump(unicode_decode(b"\xF8", 'UTF-8', U_CONV_ERROR_SKIP));
?>
--EXPECT--
unicode(0) ""

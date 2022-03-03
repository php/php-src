--TEST--
Bug #79037: global buffer-overflow in `mbfl_filt_conv_big5_wchar`
--EXTENSIONS--
mbstring
--FILE--
<?php

var_dump(mb_convert_encoding("\x81\x3a", "UTF-8", "CP950"));

?>
--EXPECT--
string(1) "?"

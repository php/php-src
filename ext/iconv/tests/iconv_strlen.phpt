--TEST--
iconv_strlen()
--EXTENSIONS--
iconv
--FILE--
<?php
function foo($str, $charset) {
    var_dump(strlen($str));
    var_dump(iconv_strlen($str, $charset));
}

foo("abc", "ASCII");
foo("ÆüËÜ¸ì EUC-JP", "EUC-JP");
?>
--EXPECT--
int(3)
int(3)
int(13)
int(10)

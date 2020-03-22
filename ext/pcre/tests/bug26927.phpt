--TEST--
Bug #26927 (preg_quote() does not escape \0)
--FILE--
<?php
    $str = "a\000b";
    $str_quoted = preg_quote($str);
    var_dump(preg_match("!{$str_quoted}!", $str), $str_quoted);
?>
--EXPECT--
int(1)
string(6) "a\000b"

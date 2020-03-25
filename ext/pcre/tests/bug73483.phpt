--TEST--
Bug #73483 (Segmentation fault on pcre_replace_callback)
--FILE--
<?php
$regex = "#dummy#";
setlocale(LC_ALL, "C");
var_dump(preg_replace_callback($regex, function (array $matches) use($regex) {
    setlocale(LC_ALL, "en_US");
    $ret = preg_replace($regex, "okey", $matches[0]);
    setlocale(LC_ALL, "C");
    return $ret;
}, "dummy"));
?>
--EXPECT--
string(4) "okey"

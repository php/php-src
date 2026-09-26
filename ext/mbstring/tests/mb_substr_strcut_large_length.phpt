--TEST--
mb_substr() and mb_strcut() with lengths that overflow when scaled to bytes
--EXTENSIONS--
mbstring
--FILE--
<?php
// len * bytes-per-char wraps to 0 for UTF-32 without the overflow check
$big = intdiv(PHP_INT_MAX, 2) + 1;
foreach (['UTF-16LE', 'UTF-32LE'] as $enc) {
    $str = mb_convert_encoding('bar', $enc, 'UTF-8');
    var_dump(mb_convert_encoding(mb_substr($str, 0, $big, $enc), 'UTF-8', $enc));
    var_dump(mb_convert_encoding(mb_substr($str, 1, PHP_INT_MAX, $enc), 'UTF-8', $enc));
}

// mb_strcut() must not size buffers or pointers from an unclamped length
foreach (['UTF-8', 'UTF-16LE', 'SJIS', 'ISO-2022-JP'] as $enc) {
    $str = mb_convert_encoding('bar', $enc, 'UTF-8');
    var_dump(mb_convert_encoding(mb_strcut($str, 0, PHP_INT_MAX, $enc), 'UTF-8', $enc));
}
?>
--EXPECT--
string(3) "bar"
string(2) "ar"
string(3) "bar"
string(2) "ar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"

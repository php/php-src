--TEST--
Unicode case mapping
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php

function toCases($str) {
    echo "String: $str\n";
    echo "Lower: ", mb_convert_case($str, MB_CASE_LOWER), "\n";
    echo "Lower Simple: ", mb_convert_case($str, MB_CASE_LOWER_SIMPLE), "\n";
    echo "Upper: ", mb_convert_case($str, MB_CASE_UPPER), "\n";
    echo "Upper Simple: ", mb_convert_case($str, MB_CASE_UPPER_SIMPLE), "\n";
    echo "Title: ", mb_convert_case($str, MB_CASE_TITLE), "\n";
    echo "Title Simple: ", mb_convert_case($str, MB_CASE_TITLE_SIMPLE), "\n";
    echo "Fold: ", mb_convert_case($str, MB_CASE_FOLD), "\n";
    echo "Fold Simple: ", mb_convert_case($str, MB_CASE_FOLD_SIMPLE), "\n";
    echo "\n";
}

toCases("ß");
toCases("ﬀ");
toCases("İ");

// Make sure that case-conversion in Turkish still works correctly.
// Using the language-agnostic Unicode case mappins would result in
// characters that are illegal under ISO-8859-9.
mb_internal_encoding('ISO-8859-9');

// Capital I with dot (U+0130)
$str = "\xdd";
echo bin2hex(mb_convert_case($str, MB_CASE_LOWER)), "\n";
echo bin2hex(mb_convert_case($str, MB_CASE_LOWER_SIMPLE)), "\n";
echo bin2hex(mb_convert_case($str, MB_CASE_FOLD)), "\n";
echo bin2hex(mb_convert_case($str, MB_CASE_FOLD_SIMPLE)), "\n";
echo "\n";

// Lower i without dot (U+0131)
$str = "\xfd";
echo bin2hex(mb_convert_case($str, MB_CASE_UPPER)), "\n";
echo bin2hex(mb_convert_case($str, MB_CASE_UPPER_SIMPLE)), "\n";
echo bin2hex(mb_convert_case($str, MB_CASE_FOLD)), "\n";
echo bin2hex(mb_convert_case($str, MB_CASE_FOLD_SIMPLE)), "\n";
echo "\n";

// Capital I without dot (U+0049)
$str = "\x49";
echo bin2hex(mb_convert_case($str, MB_CASE_LOWER)), "\n";
echo bin2hex(mb_convert_case($str, MB_CASE_LOWER_SIMPLE)), "\n";
echo bin2hex(mb_convert_case($str, MB_CASE_FOLD)), "\n";
echo bin2hex(mb_convert_case($str, MB_CASE_FOLD_SIMPLE)), "\n";
echo "\n";

// Lower i with dot (U+0069)
$str = "\x69";
echo bin2hex(mb_convert_case($str, MB_CASE_UPPER)), "\n";
echo bin2hex(mb_convert_case($str, MB_CASE_UPPER_SIMPLE)), "\n";
echo bin2hex(mb_convert_case($str, MB_CASE_FOLD)), "\n";
echo bin2hex(mb_convert_case($str, MB_CASE_FOLD_SIMPLE)), "\n";

?>
--EXPECT--
String: ß
Lower: ß
Lower Simple: ß
Upper: SS
Upper Simple: ß
Title: Ss
Title Simple: ß
Fold: ss
Fold Simple: ß

String: ﬀ
Lower: ﬀ
Lower Simple: ﬀ
Upper: FF
Upper Simple: ﬀ
Title: Ff
Title Simple: ﬀ
Fold: ff
Fold Simple: ﬀ

String: İ
Lower: i̇
Lower Simple: i
Upper: İ
Upper Simple: İ
Title: İ
Title Simple: İ
Fold: i̇
Fold Simple: İ

69
69
69
69

49
49
fd
fd

fd
fd
fd
fd

dd
dd
69
69

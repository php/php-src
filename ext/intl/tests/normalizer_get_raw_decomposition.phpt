--TEST--
normalizer_get_raw_decomposition()
--EXTENSIONS--
intl
--FILE--
<?php

$strings = [
    'a',
    "\u{FFDA}",
    "\u{FDFA}",
    '',
    'aa',
    "\xF5",
];

foreach ($strings as $string) {
    $r = Normalizer::getRawDecomposition($string, Normalizer::FORM_KC);
    if ($r === null) {
        if (intl_get_error_code() === U_ZERO_ERROR) {
            echo "'$string' has no decomposition mapping\n";
        } else {
            var_dump(intl_get_error_message());
        }
    } else {
        var_dump($r);
    }
    $r = normalizer_get_raw_decomposition($string, Normalizer::FORM_KC);
    if ($r === null) {
        if (intl_get_error_code() === U_ZERO_ERROR) {
            echo "'$string' has no decomposition mapping\n";
        } else {
            var_dump(intl_get_error_message());
        }
    } else {
        var_dump($r);
    }
}

?>
--EXPECT--
'a' has no decomposition mapping
'a' has no decomposition mapping
string(3) "ㅡ"
string(3) "ㅡ"
string(33) "صلى الله عليه وسلم"
string(33) "صلى الله عليه وسلم"
string(89) "Input string must be exactly one UTF-8 encoded code point long.: U_ILLEGAL_ARGUMENT_ERROR"
string(89) "Input string must be exactly one UTF-8 encoded code point long.: U_ILLEGAL_ARGUMENT_ERROR"
string(89) "Input string must be exactly one UTF-8 encoded code point long.: U_ILLEGAL_ARGUMENT_ERROR"
string(89) "Input string must be exactly one UTF-8 encoded code point long.: U_ILLEGAL_ARGUMENT_ERROR"
string(49) "Code point out of range: U_ILLEGAL_ARGUMENT_ERROR"
string(49) "Code point out of range: U_ILLEGAL_ARGUMENT_ERROR"

--TEST--
grapheme_strstr() handles an empty haystack without ICU errors
--EXTENSIONS--
intl
--INI--
intl.error_level=2
--FILE--
<?php
foreach ([0, 1] as $useExceptions) {
    ini_set('intl.use_exceptions', (string) $useExceptions);
    foreach (['a', "\u{00E9}", "e\u{0301}", "\r\n", ''] as $needle) {
        echo json_encode([
            grapheme_strstr('', $needle),
            grapheme_strstr('', $needle, true),
            intl_get_error_code(),
        ]), "\n";
    }
}
?>
--EXPECT--
[false,false,0]
[false,false,0]
[false,false,0]
[false,false,0]
["","",0]
[false,false,0]
[false,false,0]
[false,false,0]
[false,false,0]
["","",0]

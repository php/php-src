--TEST--
locale_get_display_keyword() throwing null bytes exceptions.
--EXTENSIONS--
intl
--FILE--
<?php

function ut_main()
{
    $calls = [
        fn() => ut_loc_get_display_keyword("cur\0rency", "fr"),
        fn() => ut_loc_get_display_keyword("currency", "f\0r"),
        fn() => ut_loc_get_display_keyword_value("de_DE@calendar=gregorian\0", "calendar", "en"),
        fn() => ut_loc_get_display_keyword_value("de_DE@calendar=gregorian", "cal\0endar", "en"),
        fn() => ut_loc_get_display_keyword_value("de_DE@calendar=gregorian", "calendar", "e\0n"),
    ];

    foreach ($calls as $call) {
        try {
            $call();
        } catch (\ValueError $e) {
            echo $e->getMessage(), PHP_EOL;
        }
    }
}

include_once 'ut_common.inc';
ut_run();
?>
--EXPECT--
Locale::getDisplayKeyword(): Argument #1 ($keyword) must not contain any null bytes
Locale::getDisplayKeyword(): Argument #2 ($displayLocale) must not contain any null bytes
Locale::getDisplayKeywordValue(): Argument #1 ($locale) must not contain any null bytes
Locale::getDisplayKeywordValue(): Argument #2 ($keyword) must not contain any null bytes
Locale::getDisplayKeywordValue(): Argument #3 ($displayLocale) must not contain any null bytes
locale_get_display_keyword(): Argument #1 ($keyword) must not contain any null bytes
locale_get_display_keyword(): Argument #2 ($displayLocale) must not contain any null bytes
locale_get_display_keyword_value(): Argument #1 ($locale) must not contain any null bytes
locale_get_display_keyword_value(): Argument #2 ($keyword) must not contain any null bytes
locale_get_display_keyword_value(): Argument #3 ($displayLocale) must not contain any null bytes

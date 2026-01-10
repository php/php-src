--TEST--
locale_get_display_language() throwing null bytes exceptions.
--EXTENSIONS--
intl
--FILE--
<?php

function ut_main()
{
    try {
	    ut_loc_get_display_language("a-D\0E", "locale=a-DE");
    } catch (\ValueError $e) {
	    echo $e->getMessage(). PHP_EOL;
    }

    try {
	    ut_loc_get_display_language("a-DE", "locale=a\0-DE");
    } catch (\ValueError $e) {
	    echo $e->getMessage(). PHP_EOL;
    }
}
include_once 'ut_common.inc';
ut_run();
?>
--EXPECT--
Locale::getDisplayLanguage(): Argument #1 ($locale) must not contain any null bytes
Locale::getDisplayLanguage(): Argument #2 ($displayLocale) must not contain any null bytes
locale_get_display_language(): Argument #1 ($locale) must not contain any null bytes
locale_get_display_language(): Argument #2 ($displayLocale) must not contain any null bytes

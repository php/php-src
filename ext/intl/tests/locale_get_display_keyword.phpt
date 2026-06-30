--TEST--
locale_get_display_keyword() basic
--EXTENSIONS--
intl
--FILE--
<?php

function ut_main()
{
    $default = ut_loc_get_default();
    ut_loc_set_default('en');

    $keyword = ut_loc_get_display_keyword('calendar', 'en');
    var_dump(is_string($keyword) && $keyword !== '');
    var_dump(ut_loc_get_display_keyword('calendar', null) === $keyword);

    $keywordValue = ut_loc_get_display_keyword_value('de_DE@calendar=gregorian', 'calendar', 'en');
    var_dump(is_string($keywordValue) && $keywordValue !== '');
    var_dump(ut_loc_get_display_keyword_value('de_DE@calendar=gregorian', 'calendar', null) === $keywordValue);

    $collationValue = ut_loc_get_display_keyword_value('de_DE@collation=phonebook', 'collation', 'en');
    var_dump(is_string($collationValue) && $collationValue !== '');

    ut_loc_set_default($default);
}

include_once 'ut_common.inc';
ut_run();
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

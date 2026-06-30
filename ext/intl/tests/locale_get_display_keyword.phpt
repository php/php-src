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
    var_dump($keyword);
    var_dump(ut_loc_get_display_keyword('calendar', null) === $keyword);

    $keywordValue = ut_loc_get_display_keyword_value('de_DE@calendar=gregorian', 'calendar', 'en');
    var_dump($keywordValue);
    var_dump(ut_loc_get_display_keyword_value('de_DE@calendar=gregorian', 'calendar', null) === $keywordValue);

    $collationValue = ut_loc_get_display_keyword_value('de_DE@collation=phonebook', 'collation', 'en');
    var_dump($collationValue);

    ut_loc_set_default($default);
}

include_once 'ut_common.inc';
ut_run();
?>
--EXPECTREGEX--
string\([1-9][0-9]*\) "[A-Za-z]+( [A-Za-z]+)*"
bool(true)
string\([1-9][0-9]*\) "[A-Za-z]+( [A-Za-z]+)*"
bool(true)
string\([1-9][0-9]*\) "[A-Za-z]+( [A-Za-z]+)*"
string\([1-9][0-9]*\) "[A-Za-z]+( [A-Za-z]+)*"
bool(true)
string\([1-9][0-9]*\) "[A-Za-z]+( [A-Za-z]+)*"
bool(true)
string\([1-9][0-9]*\) "[A-Za-z]+( [A-Za-z]+)*"

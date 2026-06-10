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

    var_dump(ut_loc_get_display_keyword('calendar', 'en'));
    var_dump(ut_loc_get_display_keyword('calendar', null));
    var_dump(ut_loc_get_display_keyword_value('de_DE@calendar=gregorian', 'calendar', 'en'));
    var_dump(ut_loc_get_display_keyword_value('de_DE@calendar=gregorian', 'calendar', null));
    var_dump(ut_loc_get_display_keyword_value('de_DE@collation=phonebook', 'collation', 'en'));

    ut_loc_set_default($default);
}

include_once 'ut_common.inc';
ut_run();
?>
--EXPECT--
string(8) "Calendar"
string(8) "Calendar"
string(18) "Gregorian Calendar"
string(18) "Gregorian Calendar"
string(20) "Phonebook Sort Order"
string(8) "Calendar"
string(8) "Calendar"
string(18) "Gregorian Calendar"
string(18) "Gregorian Calendar"
string(20) "Phonebook Sort Order"

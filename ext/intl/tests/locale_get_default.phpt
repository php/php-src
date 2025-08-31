--TEST--
locale_get_default()
--INI--
intl.default_locale=en-US
--EXTENSIONS--
intl
--FILE--
<?php

/*
 * Try getting the default Locale with different locales
 * with Procedural and Object methods.
 */

function ut_main()
{
    $res_str = '';

    $lang = ut_loc_get_default() ;
    $res_str .= "Default locale: $lang";
    $res_str .= "\n";

    locale_set_default('de-DE');
    $lang = ut_loc_get_default() ;
    $res_str .= "Default locale: $lang";
    $res_str .= "\n";

    ini_set('intl.default_locale', 'fr');
    $lang = ut_loc_get_default() ;
    $res_str .= "Default locale: $lang";
    $res_str .= "\n";

    ini_restore("intl.default_locale");

    return $res_str;

}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
Default locale: en-US
Default locale: de-DE
Default locale: fr

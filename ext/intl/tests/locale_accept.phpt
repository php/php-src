--TEST--
locale_accept_from_http
--EXTENSIONS--
intl
--FILE--
<?php

/*
 * Try parsing different Locales
 * with Procedural and Object methods.
 */

function ut_main()
{
    $res_str = "";
    $http_acc = array(
        'en-us,en;q=0.5',
        'da, en-gb;q=0.8, en;q=0.7',
        'zh, en-us;q=0.8, en;q=0.7',
        'xx, fr-FR;q=0.3, de-DE;q=0.5',
        'none',
    );

     foreach($http_acc as $http) {
        $res = ut_loc_accept_http($http);
        $res_str .= @"Accepting $http: $res\n";
    }

    return $res_str;
}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
Accepting en-us,en;q=0.5: en_US
Accepting da, en-gb;q=0.8, en;q=0.7: da
Accepting zh, en-us;q=0.8, en;q=0.7: zh
Accepting xx, fr-FR;q=0.3, de-DE;q=0.5: de_DE
Accepting none: 

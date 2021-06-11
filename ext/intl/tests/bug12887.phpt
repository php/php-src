--TEST--
locale_get_keywords() bug #12887
--EXTENSIONS--
intl
--FILE--
<?php

function ut_main()
{
    $res_str = '';
    $keywords_arr = ut_loc_get_keywords( 'de_DE@currency=EUR;collation=PHONEBOOK;sort=PHONEBOOK' );
    if ($keywords_arr) {
        foreach( $keywords_arr as $key => $value){
            $res_str .= "$key = $value\n";
        }
    }
    $res_str .= "\n";
    return $res_str;

}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
collation = PHONEBOOK
currency = EUR
sort = PHONEBOOK

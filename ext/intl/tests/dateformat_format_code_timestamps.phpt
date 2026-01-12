--TEST--
datefmt_format_code with timestamps
--EXTENSIONS--
intl
--FILE--
<?php
function ut_main() {
    $timezone = 'GMT-10:00';
    $locale_entry = 'en_US';

    $datetype_arr = [
        IntlDateFormatter::FULL,
        IntlDateFormatter::LONG,
        IntlDateFormatter::MEDIUM,
        IntlDateFormatter::SHORT,
        IntlDateFormatter::NONE
    ];

    $time_arr = [
        0,
        -1200000,
        1200000,
        2200000000.0,
        -2200000000.0,
        90099999,
        3600,
        -3600
    ];

    $res_str = '';

    foreach ($time_arr as $timestamp_entry) {
        $res_str .= "\n------------\n";
        $res_str .= "\nInput timestamp is : $timestamp_entry";
        $res_str .= "\n------------\n";

        foreach ($datetype_arr as $datetype_entry) {
            $res_str .= "\nIntlDateFormatter locale= $locale_entry ,datetype = $datetype_entry ,timetype =$datetype_entry\n";
            $fmt = ut_datefmt_create($locale_entry, $datetype_entry, $datetype_entry, $timezone, IntlDateFormatter::GREGORIAN);
            $formatted = ut_datefmt_format($fmt, $timestamp_entry);

            // Replace narrow no-break space (U+202F) with regular space for consistent output.
            // INTL doesn't seem to be very consistent about it either
            $formatted = str_replace("\u{202F}", ' ', $formatted);
            $res_str .= "Formatted timestamp is : $formatted";
        }
    }

    return $res_str;
}

include_once 'ut_common.inc';
ut_run();
?>
--EXPECT--
------------

Input timestamp is : 0
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0
Formatted timestamp is : Wednesday, December 31, 1969 at 2:00:00 PM GMT-10:00
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1
Formatted timestamp is : December 31, 1969 at 2:00:00 PM GMT-10
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2
Formatted timestamp is : Dec 31, 1969, 2:00:00 PM
IntlDateFormatter locale= en_US ,datetype = 3 ,timetype =3
Formatted timestamp is : 12/31/69, 2:00 PM
IntlDateFormatter locale= en_US ,datetype = -1 ,timetype =-1
Formatted timestamp is : 19691231 02:00 PM
------------

Input timestamp is : -1200000
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0
Formatted timestamp is : Wednesday, December 17, 1969 at 4:40:00 PM GMT-10:00
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1
Formatted timestamp is : December 17, 1969 at 4:40:00 PM GMT-10
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2
Formatted timestamp is : Dec 17, 1969, 4:40:00 PM
IntlDateFormatter locale= en_US ,datetype = 3 ,timetype =3
Formatted timestamp is : 12/17/69, 4:40 PM
IntlDateFormatter locale= en_US ,datetype = -1 ,timetype =-1
Formatted timestamp is : 19691217 04:40 PM
------------

Input timestamp is : 1200000
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0
Formatted timestamp is : Wednesday, January 14, 1970 at 11:20:00 AM GMT-10:00
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1
Formatted timestamp is : January 14, 1970 at 11:20:00 AM GMT-10
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2
Formatted timestamp is : Jan 14, 1970, 11:20:00 AM
IntlDateFormatter locale= en_US ,datetype = 3 ,timetype =3
Formatted timestamp is : 1/14/70, 11:20 AM
IntlDateFormatter locale= en_US ,datetype = -1 ,timetype =-1
Formatted timestamp is : 19700114 11:20 AM
------------

Input timestamp is : 2200000000
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0
Formatted timestamp is : Sunday, September 18, 2039 at 1:06:40 PM GMT-10:00
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1
Formatted timestamp is : September 18, 2039 at 1:06:40 PM GMT-10
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2
Formatted timestamp is : Sep 18, 2039, 1:06:40 PM
IntlDateFormatter locale= en_US ,datetype = 3 ,timetype =3
Formatted timestamp is : 9/18/39, 1:06 PM
IntlDateFormatter locale= en_US ,datetype = -1 ,timetype =-1
Formatted timestamp is : 20390918 01:06 PM
------------

Input timestamp is : -2200000000
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0
Formatted timestamp is : Saturday, April 14, 1900 at 2:53:20 PM GMT-10:00
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1
Formatted timestamp is : April 14, 1900 at 2:53:20 PM GMT-10
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2
Formatted timestamp is : Apr 14, 1900, 2:53:20 PM
IntlDateFormatter locale= en_US ,datetype = 3 ,timetype =3
Formatted timestamp is : 4/14/00, 2:53 PM
IntlDateFormatter locale= en_US ,datetype = -1 ,timetype =-1
Formatted timestamp is : 19000414 02:53 PM
------------

Input timestamp is : 90099999
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0
Formatted timestamp is : Wednesday, November 8, 1972 at 9:46:39 AM GMT-10:00
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1
Formatted timestamp is : November 8, 1972 at 9:46:39 AM GMT-10
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2
Formatted timestamp is : Nov 8, 1972, 9:46:39 AM
IntlDateFormatter locale= en_US ,datetype = 3 ,timetype =3
Formatted timestamp is : 11/8/72, 9:46 AM
IntlDateFormatter locale= en_US ,datetype = -1 ,timetype =-1
Formatted timestamp is : 19721108 09:46 AM
------------

Input timestamp is : 3600
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0
Formatted timestamp is : Wednesday, December 31, 1969 at 3:00:00 PM GMT-10:00
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1
Formatted timestamp is : December 31, 1969 at 3:00:00 PM GMT-10
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2
Formatted timestamp is : Dec 31, 1969, 3:00:00 PM
IntlDateFormatter locale= en_US ,datetype = 3 ,timetype =3
Formatted timestamp is : 12/31/69, 3:00 PM
IntlDateFormatter locale= en_US ,datetype = -1 ,timetype =-1
Formatted timestamp is : 19691231 03:00 PM
------------

Input timestamp is : -3600
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0
Formatted timestamp is : Wednesday, December 31, 1969 at 1:00:00 PM GMT-10:00
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1
Formatted timestamp is : December 31, 1969 at 1:00:00 PM GMT-10
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2
Formatted timestamp is : Dec 31, 1969, 1:00:00 PM
IntlDateFormatter locale= en_US ,datetype = 3 ,timetype =3
Formatted timestamp is : 12/31/69, 1:00 PM
IntlDateFormatter locale= en_US ,datetype = -1 ,timetype =-1
Formatted timestamp is : 19691231 01:00 PM

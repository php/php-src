--TEST--
datefmt_format_code localtime array
--EXTENSIONS--
intl
--FILE--
<?php
function ut_main()
{
    $timezone = 'GMT-10:00';
    $locale_entry = 'en_US';

    $datetype_arr = [
        IntlDateFormatter::FULL,
        IntlDateFormatter::LONG,
        IntlDateFormatter::MEDIUM,
        IntlDateFormatter::SHORT,
        IntlDateFormatter::NONE,
    ];

    $localtime_arr = [
        [
            'tm_sec' => 24,
            'tm_min' => 3,
            'tm_hour' => 19,
            'tm_mday' => 3,
            'tm_mon' => 3,
            'tm_year' => 105,
        ],
        [
            'tm_sec' => 21,
            'tm_min' => 5,
            'tm_hour' => 7,
            'tm_mday' => 13,
            'tm_mon' => 4,
            'tm_year' => 205,
        ],
        [
            'tm_sec' => 11,
            'tm_min' => 13,
            'tm_hour' => 0,
            'tm_mday' => 17,
            'tm_mon' => 11,
            'tm_year' => -5,
        ],
    ];

    $res_str = '';

    foreach ($localtime_arr as $localtime_entry) {
        $res_str .= "\n------------\n";
        $res_str .= "\nInput localtime is : ";
        $res_str .= implode(' , ', array_map(
            function ($k, $v) {
                return "$k : '$v'";
            },
            array_keys($localtime_entry),
            $localtime_entry
        ));

        $res_str .= "\n------------\n";

        foreach ($datetype_arr as $datetype_entry) {
            $res_str .= "\nIntlDateFormatter locale = $locale_entry, datetype = $datetype_entry, timetype = $datetype_entry";
            $fmt = ut_datefmt_create($locale_entry, $datetype_entry, $datetype_entry, $timezone, IntlDateFormatter::GREGORIAN);
            $formatted = ut_datefmt_format($fmt, $localtime_entry);

            // Replace narrow no-break space (U+202F) with regular space for consistent output.
            // INTL doesn't seem to be very consistent about it either
            $formatted = str_replace("\u{202F}", ' ', $formatted);

            if (intl_get_error_code() == U_ZERO_ERROR) {
                $res_str .= "\nFormatted localtime_array is : $formatted";
            } else {
                $res_str .= "\nError while formatting as: '" . intl_get_error_message() . "'";
            }
        }
    }

    return $res_str;
}

include_once 'ut_common.inc';
ut_run();
?>
--EXPECT--
------------

Input localtime is : tm_sec : '24' , tm_min : '3' , tm_hour : '19' , tm_mday : '3' , tm_mon : '3' , tm_year : '105'
------------

IntlDateFormatter locale = en_US, datetype = 0, timetype = 0
Formatted localtime_array is : Sunday, April 3, 2005 at 7:03:24 PM GMT-10:00
IntlDateFormatter locale = en_US, datetype = 1, timetype = 1
Formatted localtime_array is : April 3, 2005 at 7:03:24 PM GMT-10
IntlDateFormatter locale = en_US, datetype = 2, timetype = 2
Formatted localtime_array is : Apr 3, 2005, 7:03:24 PM
IntlDateFormatter locale = en_US, datetype = 3, timetype = 3
Formatted localtime_array is : 4/3/05, 7:03 PM
IntlDateFormatter locale = en_US, datetype = -1, timetype = -1
Formatted localtime_array is : 20050403 07:03 PM
------------

Input localtime is : tm_sec : '21' , tm_min : '5' , tm_hour : '7' , tm_mday : '13' , tm_mon : '4' , tm_year : '205'
------------

IntlDateFormatter locale = en_US, datetype = 0, timetype = 0
Formatted localtime_array is : Wednesday, May 13, 2105 at 7:05:21 AM GMT-10:00
IntlDateFormatter locale = en_US, datetype = 1, timetype = 1
Formatted localtime_array is : May 13, 2105 at 7:05:21 AM GMT-10
IntlDateFormatter locale = en_US, datetype = 2, timetype = 2
Formatted localtime_array is : May 13, 2105, 7:05:21 AM
IntlDateFormatter locale = en_US, datetype = 3, timetype = 3
Formatted localtime_array is : 5/13/05, 7:05 AM
IntlDateFormatter locale = en_US, datetype = -1, timetype = -1
Formatted localtime_array is : 21050513 07:05 AM
------------

Input localtime is : tm_sec : '11' , tm_min : '13' , tm_hour : '0' , tm_mday : '17' , tm_mon : '11' , tm_year : '-5'
------------

IntlDateFormatter locale = en_US, datetype = 0, timetype = 0
Formatted localtime_array is : Tuesday, December 17, 1895 at 12:13:11 AM GMT-10:00
IntlDateFormatter locale = en_US, datetype = 1, timetype = 1
Formatted localtime_array is : December 17, 1895 at 12:13:11 AM GMT-10
IntlDateFormatter locale = en_US, datetype = 2, timetype = 2
Formatted localtime_array is : Dec 17, 1895, 12:13:11 AM
IntlDateFormatter locale = en_US, datetype = 3, timetype = 3
Formatted localtime_array is : 12/17/95, 12:13 AM
IntlDateFormatter locale = en_US, datetype = -1, timetype = -1
Formatted localtime_array is : 18951217 12:13 AM

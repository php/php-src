--TEST--
datefmt_format_code DateTime input
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
        IntlDateFormatter::NONE
    ];

    $d1 = new DateTime("2010-01-01 01:02:03", new DateTimeZone("UTC"));
    $d2 = new DateTime("2000-12-31 03:04:05", new DateTimeZone("UTC"));
    $d2->setTimezone(new DateTimeZone("America/Los_Angeles"));

    $dates = [$d1, $d2];
    $res_str = '';

    foreach ($dates as $date_entry) {
        foreach ($datetype_arr as $datetype_entry) {
            $res_str .= "\n------------";
            $res_str .= "\nDate is: ".var_export($date_entry, true);
            $res_str .= "\n------------";

            $fmt = ut_datefmt_create($locale_entry, $datetype_entry, $datetype_entry, $timezone, IntlDateFormatter::GREGORIAN);
            $formatted = ut_datefmt_format($fmt, $date_entry);

            // Replace narrow no-break space (U+202F) with regular space for consistent output.--ARGS--
            // INTL doesn't seem to be very consistent about it either
            $formatted = str_replace("\u{202F}", ' ', $formatted);

            if (intl_get_error_code() == U_ZERO_ERROR) {
                $res_str .= "\nFormatted DateTime is : $formatted";
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
Date is: \DateTime::__set_state(array(
   'date' => '2010-01-01 01:02:03.000000',
   'timezone_type' => 3,
   'timezone' => 'UTC',
))
------------
Formatted DateTime is : Thursday, December 31, 2009 at 3:02:03 PM GMT-10:00
------------
Date is: \DateTime::__set_state(array(
   'date' => '2010-01-01 01:02:03.000000',
   'timezone_type' => 3,
   'timezone' => 'UTC',
))
------------
Formatted DateTime is : December 31, 2009 at 3:02:03 PM GMT-10
------------
Date is: \DateTime::__set_state(array(
   'date' => '2010-01-01 01:02:03.000000',
   'timezone_type' => 3,
   'timezone' => 'UTC',
))
------------
Formatted DateTime is : Dec 31, 2009, 3:02:03 PM
------------
Date is: \DateTime::__set_state(array(
   'date' => '2010-01-01 01:02:03.000000',
   'timezone_type' => 3,
   'timezone' => 'UTC',
))
------------
Formatted DateTime is : 12/31/09, 3:02 PM
------------
Date is: \DateTime::__set_state(array(
   'date' => '2010-01-01 01:02:03.000000',
   'timezone_type' => 3,
   'timezone' => 'UTC',
))
------------
Formatted DateTime is : 20091231 03:02 PM
------------
Date is: \DateTime::__set_state(array(
   'date' => '2000-12-30 19:04:05.000000',
   'timezone_type' => 3,
   'timezone' => 'America/Los_Angeles',
))
------------
Formatted DateTime is : Saturday, December 30, 2000 at 5:04:05 PM GMT-10:00
------------
Date is: \DateTime::__set_state(array(
   'date' => '2000-12-30 19:04:05.000000',
   'timezone_type' => 3,
   'timezone' => 'America/Los_Angeles',
))
------------
Formatted DateTime is : December 30, 2000 at 5:04:05 PM GMT-10
------------
Date is: \DateTime::__set_state(array(
   'date' => '2000-12-30 19:04:05.000000',
   'timezone_type' => 3,
   'timezone' => 'America/Los_Angeles',
))
------------
Formatted DateTime is : Dec 30, 2000, 5:04:05 PM
------------
Date is: \DateTime::__set_state(array(
   'date' => '2000-12-30 19:04:05.000000',
   'timezone_type' => 3,
   'timezone' => 'America/Los_Angeles',
))
------------
Formatted DateTime is : 12/30/00, 5:04 PM
------------
Date is: \DateTime::__set_state(array(
   'date' => '2000-12-30 19:04:05.000000',
   'timezone_type' => 3,
   'timezone' => 'America/Los_Angeles',
))
------------
Formatted DateTime is : 20001230 05:04 PM

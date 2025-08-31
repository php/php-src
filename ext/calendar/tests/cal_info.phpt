--TEST--
cal_info()
--INI--
date.timezone=UTC
--EXTENSIONS--
calendar
--FILE--
<?php
  print_r(cal_info());
  print_r(cal_info(1));
  try {
      cal_info(99999);
  } catch (ValueError $ex) {
      echo "{$ex->getMessage()}\n";
  }
?>
--EXPECT--
Array
(
    [0] => Array
        (
            [months] => Array
                (
                    [1] => January
                    [2] => February
                    [3] => March
                    [4] => April
                    [5] => May
                    [6] => June
                    [7] => July
                    [8] => August
                    [9] => September
                    [10] => October
                    [11] => November
                    [12] => December
                )

            [abbrevmonths] => Array
                (
                    [1] => Jan
                    [2] => Feb
                    [3] => Mar
                    [4] => Apr
                    [5] => May
                    [6] => Jun
                    [7] => Jul
                    [8] => Aug
                    [9] => Sep
                    [10] => Oct
                    [11] => Nov
                    [12] => Dec
                )

            [maxdaysinmonth] => 31
            [calname] => Gregorian
            [calsymbol] => CAL_GREGORIAN
        )

    [1] => Array
        (
            [months] => Array
                (
                    [1] => January
                    [2] => February
                    [3] => March
                    [4] => April
                    [5] => May
                    [6] => June
                    [7] => July
                    [8] => August
                    [9] => September
                    [10] => October
                    [11] => November
                    [12] => December
                )

            [abbrevmonths] => Array
                (
                    [1] => Jan
                    [2] => Feb
                    [3] => Mar
                    [4] => Apr
                    [5] => May
                    [6] => Jun
                    [7] => Jul
                    [8] => Aug
                    [9] => Sep
                    [10] => Oct
                    [11] => Nov
                    [12] => Dec
                )

            [maxdaysinmonth] => 31
            [calname] => Julian
            [calsymbol] => CAL_JULIAN
        )

    [2] => Array
        (
            [months] => Array
                (
                    [1] => Tishri
                    [2] => Heshvan
                    [3] => Kislev
                    [4] => Tevet
                    [5] => Shevat
                    [6] => Adar I
                    [7] => Adar II
                    [8] => Nisan
                    [9] => Iyyar
                    [10] => Sivan
                    [11] => Tammuz
                    [12] => Av
                    [13] => Elul
                )

            [abbrevmonths] => Array
                (
                    [1] => Tishri
                    [2] => Heshvan
                    [3] => Kislev
                    [4] => Tevet
                    [5] => Shevat
                    [6] => Adar I
                    [7] => Adar II
                    [8] => Nisan
                    [9] => Iyyar
                    [10] => Sivan
                    [11] => Tammuz
                    [12] => Av
                    [13] => Elul
                )

            [maxdaysinmonth] => 30
            [calname] => Jewish
            [calsymbol] => CAL_JEWISH
        )

    [3] => Array
        (
            [months] => Array
                (
                    [1] => Vendemiaire
                    [2] => Brumaire
                    [3] => Frimaire
                    [4] => Nivose
                    [5] => Pluviose
                    [6] => Ventose
                    [7] => Germinal
                    [8] => Floreal
                    [9] => Prairial
                    [10] => Messidor
                    [11] => Thermidor
                    [12] => Fructidor
                    [13] => Extra
                )

            [abbrevmonths] => Array
                (
                    [1] => Vendemiaire
                    [2] => Brumaire
                    [3] => Frimaire
                    [4] => Nivose
                    [5] => Pluviose
                    [6] => Ventose
                    [7] => Germinal
                    [8] => Floreal
                    [9] => Prairial
                    [10] => Messidor
                    [11] => Thermidor
                    [12] => Fructidor
                    [13] => Extra
                )

            [maxdaysinmonth] => 30
            [calname] => French
            [calsymbol] => CAL_FRENCH
        )

)
Array
(
    [months] => Array
        (
            [1] => January
            [2] => February
            [3] => March
            [4] => April
            [5] => May
            [6] => June
            [7] => July
            [8] => August
            [9] => September
            [10] => October
            [11] => November
            [12] => December
        )

    [abbrevmonths] => Array
        (
            [1] => Jan
            [2] => Feb
            [3] => Mar
            [4] => Apr
            [5] => May
            [6] => Jun
            [7] => Jul
            [8] => Aug
            [9] => Sep
            [10] => Oct
            [11] => Nov
            [12] => Dec
        )

    [maxdaysinmonth] => 31
    [calname] => Julian
    [calsymbol] => CAL_JULIAN
)
cal_info(): Argument #1 ($calendar) must be a valid calendar ID

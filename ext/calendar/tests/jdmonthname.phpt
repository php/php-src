--TEST--
jdmonthname()
--EXTENSIONS--
calendar
--FILE--
<?php
foreach (array(2440588, 2452162, 2453926) as $jd) {
  echo "### JD $jd ###\n";
  for ($mode = 0; $mode <= 6; $mode++) {
    echo "--- mode $mode ---\n";
    for ($offset = 0; $offset <= 12; $offset++) {
      echo jdmonthname($jd + $offset * 30, $mode). "\n";
    }
  }
}
?>
--EXPECT--
### JD 2440588 ###
--- mode 0 ---
Jan
Jan
Mar
Apr
May
May
Jun
Jul
Aug
Sep
Oct
Nov
Dec
--- mode 1 ---
January
January
March
April
May
May
June
July
August
September
October
November
December
--- mode 2 ---
Dec
Jan
Feb
Mar
Apr
May
Jun
Jul
Aug
Sep
Oct
Nov
Dec
--- mode 3 ---
December
January
February
March
April
May
June
July
August
September
October
November
December
--- mode 4 ---
Tevet
Shevat
Adar I
Adar II
Nisan
Iyyar
Sivan
Tammuz
Av
Elul
Tishri
Heshvan
Kislev
--- mode 5 ---













--- mode 6 ---
Jan
Jan
Mar
Apr
May
May
Jun
Jul
Aug
Sep
Oct
Nov
Dec
### JD 2452162 ###
--- mode 0 ---
Sep
Oct
Nov
Dec
Jan
Feb
Mar
Apr
May
Jun
Jul
Aug
Sep
--- mode 1 ---
September
October
November
December
January
February
March
April
May
June
July
August
September
--- mode 2 ---
Aug
Sep
Oct
Nov
Dec
Jan
Feb
Mar
Apr
May
Jun
Jul
Aug
--- mode 3 ---
August
September
October
November
December
January
February
March
April
May
June
July
August
--- mode 4 ---
Elul
Tishri
Heshvan
Kislev
Tevet
Shevat
Adar
Nisan
Iyyar
Sivan
Tammuz
Av
Elul
--- mode 5 ---













--- mode 6 ---
Sep
Oct
Nov
Dec
Jan
Feb
Mar
Apr
May
Jun
Jul
Aug
Sep
### JD 2453926 ###
--- mode 0 ---
Jul
Aug
Sep
Oct
Nov
Dec
Jan
Feb
Mar
Apr
May
Jun
Jul
--- mode 1 ---
July
August
September
October
November
December
January
February
March
April
May
June
July
--- mode 2 ---
Jun
Jul
Aug
Sep
Oct
Nov
Dec
Jan
Feb
Mar
Apr
May
Jun
--- mode 3 ---
June
July
August
September
October
November
December
January
February
March
April
May
June
--- mode 4 ---
Tammuz
Av
Elul
Tishri
Heshvan
Kislev
Tevet
Shevat
Adar
Nisan
Iyyar
Sivan
Tammuz
--- mode 5 ---













--- mode 6 ---
Jul
Aug
Sep
Oct
Nov
Dec
Jan
Feb
Mar
Apr
May
Jun
Jul

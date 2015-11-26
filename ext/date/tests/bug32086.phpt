--TEST--
Bug #32086 (strtotime don't work in DST)
--INI--
date.timezone=America/Sao_Paulo
--FILE--
<?php

echo $g = strtotime("2004-11-01"), "\n";
echo $i = strtotime("2004-11-01 +1 day"), "\n";
echo $j = strtotime("+1 day", $g), "\n";
echo $k = strtotime("2004-11-02"), "\n";
echo $l = strtotime("2004-11-03"), "\n";
echo date("Y-m-d H:i:s T\n", $g);
echo date("Y-m-d H:i:s T\n", $i);
echo date("Y-m-d H:i:s T\n", $j);
echo date("Y-m-d H:i:s T\n", $k);
echo date("Y-m-d H:i:s T\n", $l);

echo $g = strtotime("2005-02-19"), "\n";
echo $i = strtotime("2005-02-19 +1 day"), "\n";
echo $j = strtotime("+1 day", $g), "\n";
echo $k = strtotime("2005-02-20"), "\n";
echo $l = strtotime("2005-02-21"), "\n";
echo date("Y-m-d H:i:s T\n", $g);
echo date("Y-m-d H:i:s T\n", $i);
echo date("Y-m-d H:i:s T\n", $j);
echo date("Y-m-d H:i:s T\n", $k);
echo date("Y-m-d H:i:s T\n", $l);

?>
--EXPECT--
1099278000
1099364400
1099364400
1099364400
1099447200
2004-11-01 00:00:00 BRT
2004-11-02 01:00:00 BRST
2004-11-02 01:00:00 BRST
2004-11-02 01:00:00 BRST
2004-11-03 00:00:00 BRST
1108778400
1108868400
1108868400
1108868400
1108954800
2005-02-19 00:00:00 BRST
2005-02-20 00:00:00 BRT
2005-02-20 00:00:00 BRT
2005-02-20 00:00:00 BRT
2005-02-21 00:00:00 BRT

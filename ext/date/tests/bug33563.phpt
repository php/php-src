--TEST--
Bug #33563 (strtotime('+1 month',$abc) cant get right time)
--FILE--
<?php
date_default_timezone_set("GMT");
$strCurrDate = date('Y-m-d H:i:s',strtotime('2005-06-30 21:04:23'));
$strMonAfter = date('Y-m-d H:i:s',strtotime('+1 month',strtotime($strCurrDate)));

echo "strCurrDate:$strCurrDate strMonAfter:$strMonAfter";
?>
--EXPECT--
strCurrDate:2005-06-30 21:04:23 strMonAfter:2005-07-30 21:04:23

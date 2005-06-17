--TEST--
Bug #32086 (strtotime don't work in DST)
--FILE--
<?php
putenv("TZ=America/Sao_Paulo");
echo $i = strtotime("2004-11-01"), "\n";
echo strtotime("+1 day", $i), "\n";
echo $i = strtotime("2005-02-19"), "\n";
echo strtotime("+1 day", $i), "\n";
?>
--EXPECT--
1099278000
1099360800
1108778400
1108868400

--TEST--
Bug #33056 (strtotime() does not parse 20050518t090000Z)
--FILE--
<?php
date_default_timezone_set("GMT");
echo strtotime('20050518t090000Z')."\n";
echo strtotime('20050518t091234Z')."\n";
echo strtotime('20050518t191234Z')."\n";
?>
--EXPECT--
1116406800
1116407554
1116443554

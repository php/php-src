--TEST--
Bug #29595 (Roman number format for months)
--FILE--
<?php
date_default_timezone_set("GMT");
$from_postgres = '2004-08-09 14:48:27.304809+10';

echo strtotime($from_postgres);
?>
--EXPECT--
1092026907

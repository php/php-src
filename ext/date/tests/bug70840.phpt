--TEST--
Bug #52577 (Incorrect date returning)
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--FILE--
<?php
date_default_timezone_set('Europe/Zaporizhia');
$date = '05.11.2015';
echo "String: ".$date."\n";
$date_format = DATE_RFC2822;
$unixtime = strtotime($date);
echo "Unixtime: ".$unixtime."\n";
echo "Date(PHP): ".date($date_format,$unixtime)."\n";
$date = new DateTime('@'.$unixtime);
echo "DateTime(PHP Class): ".$date->format($date_format);
?>
--EXPECT--
String: 05.11.2015
Unixtime: 1446674400
Date(PHP): Thu, 05 Nov 2015 00:00:00 +0200
DateTime(PHP Class): Wed, 04 Nov 2015 22:00:00 +0000

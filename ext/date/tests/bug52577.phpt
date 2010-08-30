--TEST--
Bug #52577 (Incorrect date returning)
--FILE--
<?php
date_default_timezone_set('Europe/Kiev');
$date = '7.8.2010';
echo "String: ".$date."\n";
$date_format = DATE_RFC2822;
$unixtime = strtotime($date);
echo "Unixtime: ".$unixtime."\n";
echo "Date(PHP): ".date($date_format,$unixtime)."\n";
$date = new DateTime('@'.$unixtime);
echo "DateTime(PHP Class): ".$date->format($date_format);
?>
--EXPECT--
String: 7.8.2010
Unixtime: 1281128400
Date(PHP): Sat, 07 Aug 2010 00:00:00 +0300
DateTime(PHP Class): Fri, 06 Aug 2010 21:00:00 +0000

--TEST--
Bug #54851 (DateTime::createFromFormat() doesn't interpret "D")
--INI--
date.timezone=UTC
--FILE--
<?php
$date = new DateTime("2011-05-17T22:14:12");
$date2 = DateTime::createFromFormat("D H i s", $date->format("D"). ' 0 00 00');
echo $date->format("r"), "\n";
echo $date2->format("r"), "\n";
var_dump($date->format("D") == $date2->format("D"));

// Verify that our implementation works regardless of position
$datePre = DateTime::createFromFormat("!D d M Y", "Fri 17 may 2011");
$datePost = DateTime::createFromFormat("!d M Y D", "17 may 2011 Fri");
echo $datePre->format("r"), "\n";
echo $datePost->format("r"), "\n";
var_dump($datePre->format("Y-m-d") == $datePost->format("Y-m-d"));

// Verify that our implementation is the same as for the constructor and
// strtotime
$date1 = new DateTime("Tuesday");
$date2 = DateTime::createFromFormat("D H i s", "Tuesday 0 00 00");
echo $date1->format('r'), "\n";
echo $date2->format('r'), "\n";
var_dump($date1->format('D') == $date2->format('D'));

// - when the day is not the same as the day on the original date:
$date1 = DateTime::createFromFormat("!D d M Y", "Fri 19 November 2011");
$date2 = new DateTime("Fri 19 November 2011");
echo $date1->format('r'), "\n";
echo $date2->format('r'), "\n";
var_dump($date1->format('Y-m-d') == $date2->format('Y-m-d'));

// - when the day *is* the same as the day on the original date:
$date1 = DateTime::createFromFormat("!D d M Y", "Sat 19 November 2011");
$date2 = new DateTime("Sat 19 November 2011");
echo $date1->format('r'), "\n";
echo $date2->format('r'), "\n";
var_dump($date1->format('Y-m-d') == $date2->format('Y-m-d'));
?>
--EXPECTF--
Tue, 17 May 2011 22:14:12 +0000
Tue, %d %s %d 00:00:00 +0000
bool(true)
Fri, 20 May 2011 00:00:00 +0000
Fri, 20 May 2011 00:00:00 +0000
bool(true)
Tue, %d %s %d 00:00:00 +0000
Tue, %d %s %d 00:00:00 +0000
bool(true)
Fri, 25 Nov 2011 00:00:00 +0000
Fri, 25 Nov 2011 00:00:00 +0000
bool(true)
Sat, 19 Nov 2011 00:00:00 +0000
Sat, 19 Nov 2011 00:00:00 +0000
bool(true)

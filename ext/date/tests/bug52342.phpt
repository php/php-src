--TEST--
Bug #52342 (DateTime setIsoDate results in wrong timestamp)
--FILE--
<?php
date_default_timezone_set('Europe/Berlin');
$from = new DateTime();
$from->setTime(0, 0, 0);
$from->setISODate(2010, 28, 1); //Montag der 28ten Woche 2010
 
echo $from->format('d.m.Y H:i'), "\n"; //A
echo $from->getTimestamp(), "\n"; //B
echo date('d.m.Y H:i', $from->getTimestamp()), "\n"; //C

$from->add(new DateInterval('P0D'));
echo $from->getTimestamp(), "\n"; //B
echo date('d.m.Y H:i', $from->getTimestamp()), "\n"; //C
?>
--EXPECT--
12.07.2010 00:00
1278885600
12.07.2010 00:00
1278885600
12.07.2010 00:00

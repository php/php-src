--TEST--
Bug GH-21616 (DateTime::modify() does not respect DST transitions)
--FILE--
<?php
/* Spring forward: Europe/London, 2025-03-30 01:00 GMT -> 02:00 BST */
$tz = new DateTimeZone('Europe/London');

/* +1s then -1s must round-trip */
$dt = new DateTime('2025-03-30 00:59:59', $tz);
$dt->modify('+1 second');
$dt->modify('-1 second');
echo $dt->format('Y-m-d H:i:s T U'), "\n";

/* -1s from 02:00 BST must land at 00:59:59 GMT, not 02:59:59 BST */
$dt2 = new DateTime('2025-03-30 02:00:00', $tz);
echo $dt2->modify('-1 second')->format('Y-m-d H:i:s T U'), "\n";

/* month + hours: +1 month lands before the DST boundary, so +1 hour is plain GMT */
$dt3 = new DateTime('2025-02-28 00:30:00', $tz);
echo $dt3->modify('+1 month +1 hour')->format('Y-m-d H:i:s T U'), "\n";

/* first/last day of must still work */
$base = new DateTimeImmutable('2025-03-15 10:00:00', $tz);
echo $base->modify('first day of next month')->format('Y-m-d H:i:s T'), "\n";
echo $base->modify('last day of this month')->format('Y-m-d H:i:s T'), "\n";

/* +61 minutes from just before the gap -- minutes must also count as elapsed time */
$dt4 = new DateTime('2025-03-30 00:59:00', $tz);
echo $dt4->modify('+61 minutes')->format('Y-m-d H:i:s T U'), "\n";

/* DateTimeImmutable must behave the same as mutable DateTime */
$dt5 = new DateTimeImmutable('2025-03-30 02:00:00', $tz);
echo $dt5->modify('-1 second')->format('Y-m-d H:i:s T U'), "\n";
?>
--EXPECT--
2025-03-30 00:59:59 GMT 1743296399
2025-03-30 00:59:59 GMT 1743296399
2025-03-28 01:30:00 GMT 1743125400
2025-04-01 10:00:00 BST
2025-03-31 10:00:00 BST
2025-03-30 03:00:00 BST 1743300000
2025-03-30 00:59:59 GMT 1743296399

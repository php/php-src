--TEST--
Bug GH-15880 (DateTime::modify('+72 hours') incorrect across DST boundary)
--FILE--
<?php
/* Fall back: America/Chicago, 2024-11-03 02:00 CDT -> 01:00 CST.
 * +72 hours from midnight Nov 1 must land at Nov 3 23:00 CST, not Nov 4 00:00. */
date_default_timezone_set('America/Chicago');
$tz = new DateTimeZone('America/Chicago');
$start = new DateTimeImmutable('2024-11-01 00:00:00', $tz);

/* modify and add must agree */
echo $start->modify('+72 hours')->format('Y-m-d H:i:s T U'), "\n";
echo $start->add(new DateInterval('PT72H'))->format('Y-m-d H:i:s T U'), "\n";

/* +3 days is calendar arithmetic -- it should land on midnight Nov 4 */
echo $start->modify('+3 days')->format('Y-m-d H:i:s T U'), "\n";

/* -72 hours backward through fall-back: 73 real hours separate Nov 1 00:00 CDT
 * from Nov 4 00:00 CST (the extra hour is the repeated hour), so -72h lands 1h
 * ahead of Nov 1 midnight */
$end = new DateTimeImmutable('2024-11-04 00:00:00', $tz);
echo $end->modify('-72 hours')->format('Y-m-d H:i:s T U'), "\n";
?>
--EXPECT--
2024-11-03 23:00:00 CST 1730696400
2024-11-03 23:00:00 CST 1730696400
2024-11-04 00:00:00 CST 1730700000
2024-11-01 01:00:00 CDT 1730440800

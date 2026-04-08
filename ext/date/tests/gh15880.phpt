--TEST--
Bug GH-15880 (DateTime::modify('+72 hours') incorrect across DST boundary)
--FILE--
<?php
/* Fall back: America/Chicago, 2024-11-03 02:00 CDT -> 01:00 CST.
 * +72 hours from 2024-11-01 00:00 CDT must be 2024-11-03 23:00 CST,
 * not 2024-11-04 00:00 CST. */
$tz = new DateTimeZone('America/Chicago');

echo "=== Fall back (explicit timezone) ===\n";

$start = new DateTimeImmutable('2024-11-01 00:00:00', $tz);

echo "modify +72 hours:\n";
echo $start->modify('+72 hours')->format('Y-m-d H:i:s T U'), "\n";

echo "add PT72H (reference):\n";
echo $start->add(new DateInterval('PT72H'))
           ->format('Y-m-d H:i:s T U'), "\n";

echo "\n=== Fall back with default timezone ===\n";

/* Exact reproduction from GH-15880: date_default_timezone_set,
 * DateTimeImmutable, +72 hour via modify vs add. Also test that
 * +3 days (calendar arithmetic) is unaffected — it should land on
 * midnight Nov 4, not Nov 3 23:00. */
date_default_timezone_set('America/Chicago');
$start2 = new DateTimeImmutable('2024-11-01');

echo "modify +72 hour:\n";
echo $start2->modify('+72 hour')
            ->format('Y-m-d H:i:s T U'), "\n";

echo "add PT72H (reference):\n";
echo $start2->add(new DateInterval('PT72H'))
            ->format('Y-m-d H:i:s T U'), "\n";

echo "modify +3 days (calendar, not hours):\n";
echo $start2->modify('+3 days')
            ->format('Y-m-d H:i:s T U'), "\n";
?>
--EXPECT--
=== Fall back (explicit timezone) ===
modify +72 hours:
2024-11-03 23:00:00 CST 1730696400
add PT72H (reference):
2024-11-03 23:00:00 CST 1730696400

=== Fall back with default timezone ===
modify +72 hour:
2024-11-03 23:00:00 CST 1730696400
add PT72H (reference):
2024-11-03 23:00:00 CST 1730696400
modify +3 days (calendar, not hours):
2024-11-04 00:00:00 CST 1730700000

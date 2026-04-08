--TEST--
Bug GH-21616 (DateTime::modify() does not respect DST transitions)
--FILE--
<?php
/* Spring forward: Europe/London, 2025-03-30 01:00 GMT -> 02:00 BST.
 * Subtracting 1 second from 02:00:00 BST must land at 00:59:59 GMT,
 * not at 02:59:59 BST. */
$tz = new DateTimeZone('Europe/London');

echo "=== Spring forward ===\n";

echo "modify +1s then -1s:\n";
$dt = new DateTime('2025-03-30 00:59:59', $tz);
$dt->modify('+1 second');
$dt->modify('-1 second');
echo $dt->format('Y-m-d H:i:s T U'), "\n";

echo "add/sub PT1S (reference):\n";
$dt2 = new DateTime('2025-03-30 00:59:59', $tz);
$dt2->add(new DateInterval('PT1S'));
$dt2->sub(new DateInterval('PT1S'));
echo $dt2->format('Y-m-d H:i:s T U'), "\n";

echo "modify -1s from 02:00 BST:\n";
$dt3 = new DateTime('2025-03-30 02:00:00', $tz);
echo $dt3->modify('-1 second')->format('Y-m-d H:i:s T U'), "\n";

echo "\n=== Combined relative (month + hours near DST) ===\n";

/* 2025-02-28 00:30:00 GMT + 1 month + 1 hour should land on
 * 2025-03-28 01:30:00 GMT (not affected by DST on March 30). */
$dt4 = new DateTime('2025-02-28 00:30:00', $tz);
echo "modify +1 month +1 hour:\n";
echo $dt4->modify('+1 month +1 hour')->format('Y-m-d H:i:s T U'), "\n";

echo "\n=== first/last day of (must not regress) ===\n";

/* Ensure first_last_day_of still works correctly. */
$dt5 = new DateTime('2025-03-15 10:00:00', $tz);
echo "modify first day of next month:\n";
echo $dt5->modify('first day of next month')
         ->format('Y-m-d H:i:s T'), "\n";

$dt6 = new DateTime('2025-03-15 10:00:00', $tz);
echo "modify last day of this month:\n";
echo $dt6->modify('last day of this month')
         ->format('Y-m-d H:i:s T'), "\n";
?>
--EXPECT--
=== Spring forward ===
modify +1s then -1s:
2025-03-30 00:59:59 GMT 1743296399
add/sub PT1S (reference):
2025-03-30 00:59:59 GMT 1743296399
modify -1s from 02:00 BST:
2025-03-30 00:59:59 GMT 1743296399

=== Combined relative (month + hours near DST) ===
modify +1 month +1 hour:
2025-03-28 01:30:00 GMT 1743125400

=== first/last day of (must not regress) ===
modify first day of next month:
2025-04-01 10:00:00 BST
modify last day of this month:
2025-03-31 10:00:00 BST

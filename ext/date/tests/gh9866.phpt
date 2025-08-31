--TEST--
Bug GH-9866 (Time zone bug with \DateTimeInterface::diff())
--FILE--
<?php
function getYearsBetween(
    \DateTimeImmutable $startDate,
    \DateTimeImmutable $endDate,
): int {
    $dateInterval = $startDate->diff($endDate, true);
    return $dateInterval->y;
}

$start = new \DateTimeImmutable('2000-11-01 09:29:22.907606', new \DateTimeZone('America/Chicago'));
$end = new \DateTimeImmutable('2022-06-06 11:00:00.000000', new \DateTimeZone('America/New_York'));
$result = getYearsBetween($start, $end);
var_dump($result);
$diff = $start->diff($end);
echo $diff->format("%R %Y %M %D (%a) %H %I %S %F"), "\n";
?>
--EXPECT--
int(21)
+ 21 07 04 (7886) 23 30 37 092394

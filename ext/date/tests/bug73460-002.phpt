--TEST--
Bug #73460 (Datetime add not realising it already applied DST change)
--FILE--
<?php

date_default_timezone_set('America/New_York');

//DST starts Apr. 2nd 02:00 and moves to 03:00
$start = new \DateTime('2006-04-02T01:00:00');
$end = new \DateTime('2006-04-02T04:00:00');

while($end > $start) {
    $now = clone $end;
    $end->sub(new \DateInterval('PT1H'));
    echo $end->format('Y-m-d H:i T') . PHP_EOL;
}

echo '-----' . \PHP_EOL;

//DST ends Oct. 29th 02:00 and moves to 01:00
$start = new \DateTime('2006-10-29T00:30:00');
$end = new \DateTime('2006-10-29T03:00:00');

$i = 0;
while($end > $start) {
    $now = clone $start;
    $start->add(new \DateInterval('PT30M'));
    echo $start->format('Y-m-d H:i T') . PHP_EOL;
}
?>
--EXPECT--
2006-04-02 03:00 EDT
2006-04-02 01:00 EST
-----
2006-10-29 01:00 EDT
2006-10-29 01:30 EDT
2006-10-29 01:00 EST
2006-10-29 01:30 EST
2006-10-29 02:00 EST
2006-10-29 02:30 EST
2006-10-29 03:00 EST

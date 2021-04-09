--TEST--
Bug #74173 (DateTimeImmutable::getTimestamp() triggers DST switch in incorrect time)
--FILE--
<?php
$utc = new \DateTimeImmutable('2016-10-30T00:00:00+00:0');

$prg = $utc->setTimeZone(new \DateTimeZone('Europe/Prague'));
echo $prg->format('c') . "\n";
$prg->getTimestamp();
echo $prg->format('c') . "\n";
?>
--EXPECT--
2016-10-30T02:00:00+02:00
2016-10-30T02:00:00+02:00

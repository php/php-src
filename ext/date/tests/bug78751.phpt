--TEST--
Bug #78751 (Serialising DatePeriod converts DateTimeImmutable)
--FILE--
<?php
$oDay = new DateTimeImmutable('2019-10-25');
$oDateInterval = DateInterval::createFromDateString('1 day');
$oDays = new DatePeriod($oDay, $oDateInterval, $oDay->modify('+1 day'));
$oDays = unserialize(serialize($oDays));
var_dump(
    $oDays->start instanceof DateTimeImmutable,
    $oDays->end instanceof DateTimeImmutable
);
?>
--EXPECT--
bool(true)
bool(true)

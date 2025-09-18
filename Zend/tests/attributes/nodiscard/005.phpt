--TEST--
#[\NoDiscard]: Native method.
--FILE--
<?php

$date = new DateTimeImmutable('now');
$date->setTimestamp(0);

?>
--EXPECTF--
Warning: The return value of method DateTimeImmutable::setTimestamp() should either be used or intentionally ignored by casting it as (void), as DateTimeImmutable::setTimestamp() does not modify the object itself in %s on line %d

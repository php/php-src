--TEST--
#[\NoDiscard]: Native function and method.
--FILE--
<?php

$f = tmpfile();
flock($f, LOCK_SH | LOCK_NB);
fclose($f);

$date = new DateTimeImmutable('now');
$date->setTimestamp(0);

?>
--EXPECTF--
Warning: The return value of function flock() should either be used or intentionally ignored by casting it as (void), as locking the stream might have failed in %s on line %d

Warning: The return value of method DateTimeImmutable::setTimestamp() should either be used or intentionally ignored by casting it as (void), as DateTimeImmutable::setTimestamp() does not modify the object itself in %s on line %d

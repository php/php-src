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
Warning: (B)The return value of function flock() is expected to be consumed, as locking the stream might have failed in %s on line %d

Warning: (A)The return value of method DateTimeImmutable::setTimestamp() is expected to be consumed, as DateTimeImmutable::setTimestamp() does not modify the object itself in %s on line %d

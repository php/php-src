--TEST--
DateInterval: Getter and setter errors
--FILE--
<?php
$d = DateInterval::createFromDateString("");
$d->y = 1984;
var_dump($d->y);
var_dump($d->asdf, $d->y);
?>
--EXPECTF--
int(1984)

Fatal error: main(): Unknown property (asdf) in %sdateinterval_errors.php on line 5

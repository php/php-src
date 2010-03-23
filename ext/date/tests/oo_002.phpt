--TEST--
date OO cloning
--FILE--
<?php
date_default_timezone_set('Europe/Berlin');
class _d extends DateTime {}
class _t extends DateTimeZone {}
$d = new _d("1pm Aug 1 GMT 2007");
var_dump($d->format(DateTime::RFC822));
$c = clone $d;
var_dump($c->format(DateTime::RFC822));
$d->modify("1 hour");
$c->modify("1 second ago");
var_dump($d->format(DateTime::RFC822));
var_dump($c->format(DateTime::RFC822));
$t = new _t("Asia/Tokyo");
var_dump($t->getName());
$c = clone $t;
var_dump($c->getName());
?>
--EXPECT--
string(29) "Wed, 01 Aug 07 13:00:00 +0000"
string(29) "Wed, 01 Aug 07 13:00:00 +0000"
string(29) "Wed, 01 Aug 07 14:00:00 +0000"
string(29) "Wed, 01 Aug 07 12:59:59 +0000"
string(10) "Asia/Tokyo"
string(10) "Asia/Tokyo"

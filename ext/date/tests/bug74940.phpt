--TEST--
Bug #74940 DateTimeZone loose comparison always true until properties are initialized
--INI--
date.timezone=UTC
--FILE--
<?php

$tz1 = new DateTimeZone('Europe/Amsterdam');
$tz2 = new DateTimeZone('UTC');

var_dump($tz1 == $tz2);
var_dump($tz1 === $tz2);

$tz3 = clone($tz1);
$tz4 = clone($tz2);

var_dump($tz1 == $tz3);
var_dump($tz4 == $tz3);

$tz5 = unserialize(serialize($tz1));
var_dump($tz1 == $tz5);
var_dump($tz2 == $tz5);
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)


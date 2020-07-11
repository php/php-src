--TEST--
Failing test case for #75857: Long timezones truncation
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--FILE--
<?php
$longDate = new DateTime('now', new DateTimeZone('America/Argentina/ComodRivadavia'));
$mediumDate = new DateTime('now', new DateTimeZone('America/Indiana/Indianapolis'));
$smallDate = new DateTime('now', new DateTimeZone('America/Sao_Paulo'));

var_dump($longDate->format('e'));
var_dump($mediumDate->format('e'));
var_dump($smallDate->format('e'));
?>
--EXPECT--
string(32) "America/Argentina/ComodRivadavia"
string(28) "America/Indiana/Indianapolis"
string(17) "America/Sao_Paulo"

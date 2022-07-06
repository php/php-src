--TEST--
MessageFormatter::format(): DateTime accepted to format dates and times
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
//ini_set("intl.default_locale", "nl");

$fmt = <<<EOD
{0,date} {0,time}
EOD;

$dt = new DateTime("2012-05-06 18:00:42", new DateTimeZone("Europe/Lisbon"));
$dti = new DateTimeImmutable("2012-05-06 18:00:42", new DateTimeZone("Europe/Lisbon"));

$mf = new MessageFormatter('en_US', $fmt);

var_dump($mf->format(array($dt)));
var_dump($mf->format(array($dti)));

?>
--EXPECTF--
string(%s) "May %d, 2012 %d:%d:42 %s"
string(%s) "May %d, 2012 %d:%d:42 %s"

--TEST--
MessageFormatter::format(): DateTime accepted to format dates and times
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '72.1') < 0) die('skip for ICU >= 72.1'); ?>
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
--EXPECT--
string(24) "May 6, 2012 5:00:42 PM"
string(24) "May 6, 2012 5:00:42 PM"

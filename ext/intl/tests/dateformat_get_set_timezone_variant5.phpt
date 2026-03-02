--TEST--
IntlDateFormatter: get/setTimeZone()
--EXTENSIONS--
intl
--INI--
intl.default_locale=pt_PT
date.timezone=Atlantic/Azores
--FILE--
<?php

$ts = strtotime('2012-01-01 00:00:00 UTC');

function d(IntlDateFormatter $df) {
global $ts;
echo $df->format($ts), "\n";
var_dump(
$df->getTimeZoneID(),
$df->getTimeZone()->getID());
echo "\n";
}

$df = new IntlDateFormatter('pt_PT', 0, 0, 'Europe/Minsk');
d($df);

$df->setTimeZone(NULL);
d($df);

$df->setTimeZone('Europe/Madrid');
d($df);

$df->setTimeZone(IntlTimeZone::createTimeZone('Europe/Paris'));
d($df);

$df->setTimeZone(new DateTimeZone('Europe/Amsterdam'));
d($df);

?>
--EXPECTF--
domingo, 1 de janeiro de 2012 às 03:00:00 Hor%s do Extremo Leste da Europa
string(12) "Europe/Minsk"
string(12) "Europe/Minsk"

sábado, 31 de dezembro de 2011 às 23:00:00 Hor%s padrão %Sdos Açores
string(15) "Atlantic/Azores"
string(15) "Atlantic/Azores"

domingo, 1 de janeiro de 2012 às 01:00:00 Hor%s padrão %Sda Europa Central
string(13) "Europe/Madrid"
string(13) "Europe/Madrid"

domingo, 1 de janeiro de 2012 às 01:00:00 Hor%s padrão %Sda Europa Central
string(12) "Europe/Paris"
string(12) "Europe/Paris"

domingo, 1 de janeiro de 2012 às 01:00:00 Hor%s padrão %Sda Europa Central
string(16) "Europe/Amsterdam"
string(16) "Europe/Amsterdam"


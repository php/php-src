--TEST--
MessageFormat accepts IntlCalendar args
--EXTENSIONS--
intl
--INI--
date.timezone=Europe/Lisbon
--SKIPIF--
<?php
if (str_contains(PHP_OS, 'FreeBSD')) {
    die('xfail Fails on FreeBSD for unknown reason');
}
?>
--FILE--
<?php

$cal = new IntlGregorianCalendar(2012,04,17,17,35,36);

$msgf = new MessageFormatter('pt_PT', '{0,date,full} {0,time,h:m:s a V}');
echo $msgf->format(array($cal)), "\n";

//NOT FIXED:
/*$msgf = new MessageFormatter('en_US',
'{1, select, date {{0,date,full}} other {{0,time,h:m:s a V}}}');

echo "msgf2: ", $msgf->format(array($time, 'date')), " ",
        $msgf->format(array($time, 'time')), "\n";
*/

?>
--EXPECTF--
Deprecated: Calling IntlGregorianCalendar::__construct() with more than 2 arguments is deprecated, use either IntlGregorianCalendar::createFromDate() or IntlGregorianCalendar::createFromDateTime() instead in %s on line %d
quinta-feira, 17 de maio de 2012 5:35:36 da tarde ptlis

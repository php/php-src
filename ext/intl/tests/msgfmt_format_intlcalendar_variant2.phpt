--TEST--
MessageFormat accepts IntlCalendar args
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '51.2') || version_compare(INTL_ICU_VERSION, '52.1') >= 0) die('skip for ICU >= 51.2 and < 52.1'); ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
//ini_set("intl.default_locale", "nl");
ini_set('date.timezone', 'Europe/Lisbon');

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
--EXPECT--
Quinta-feira, 17 de Maio de 2012 5:35:36 PM ptlis

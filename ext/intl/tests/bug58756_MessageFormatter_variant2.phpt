--TEST--
Bug #58756: w.r.t MessageFormatter
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '51.2') < 0)
	die('skip for ICU >= 51.2');
?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
//ini_set("intl.default_locale", "nl");

$time = 1247013673;

ini_set('date.timezone', 'America/New_York');

$msgf = new MessageFormatter('en_US', '{0,date,full} {0,time,h:m:s a V}');

echo "date:  " . date('l, F j, Y g:i:s A T', $time) . "\n";
echo "msgf:  " . $msgf->format(array($time)) . "\n";

//NOT FIXED:
/*$msgf = new MessageFormatter('en_US',
'{1, select, date {{0,date,full}} other {{0,time,h:m:s a V}}}');

echo "msgf2: ", $msgf->format(array($time, 'date')), " ",
		$msgf->format(array($time, 'time')), "\n";
*/

?>
==DONE==
--EXPECT--
date:  Tuesday, July 7, 2009 8:41:13 PM EDT
msgf:  Tuesday, July 7, 2009 8:41:13 PM usnyc
==DONE==

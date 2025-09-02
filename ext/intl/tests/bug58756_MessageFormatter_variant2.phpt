--TEST--
Bug #58756: w.r.t MessageFormatter
--EXTENSIONS--
intl
--INI--
date.timezone=America/New_York
--SKIPIF--
<?php
if (str_contains(PHP_OS, 'FreeBSD')) {
    die('xfail Fails on FreeBSD for unknown reason');
}
?>
--FILE--
<?php

$time = 1247013673;

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
--EXPECT--
date:  Tuesday, July 7, 2009 8:41:13 PM EDT
msgf:  Tuesday, July 7, 2009 8:41:13 PM usnyc

--TEST--
Format timestamp in DST test
--INI--
date.timezone=CEST
--FILE--
<?php
error_reporting(E_ALL & ~E_STRICT); // hide e_strict warning about timezones
var_dump( date_create( '@1202996091' )->format( 'c' ) );
?>
--EXPECT--
unicode(25) "2008-02-14T13:34:51+00:00"

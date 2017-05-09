--TEST--
Format timestamp in DST test
--INI--
date.timezone=UTC
--FILE--
<?php
var_dump( date_create( '@1202996091' )->format( 'c' ) );
?>
--EXPECTF--
Warning: date_create(): Invalid date.timezone value 'CEST', we selected the timezone 'UTC' for now. in %s on line %d
string(25) "2008-02-14T13:34:51+00:00"

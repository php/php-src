--TEST--
Format timestamp in DST test
--INI--
date.timezone=UTC
--FILE--
<?php
var_dump( date_create( '@1202996091' )->format( 'c' ) );
?>
--EXPECT--
string(25) "2008-02-14T13:34:51+00:00"

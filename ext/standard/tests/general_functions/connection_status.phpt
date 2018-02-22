--TEST--
int connection_status ( void );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--FILE--
<?php
var_dump(connection_status() == CONNECTION_NORMAL);
?>
--EXPECT--
bool(true)

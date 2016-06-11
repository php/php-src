--TEST--
int connection_aborted ( void );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--FILE--
<?php
var_dump(connection_aborted());
?>
--EXPECTF--
int(0)

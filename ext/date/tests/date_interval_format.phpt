--TEST--
Test date_interval_format() function : basic functionality
--CREDITS--
Jean Carlo Machado <contato@jeancarlomachado.com.br>
--INI--
date.timezone=UTC
--FILE--
<?php

$interval = new DateInterval('P2Y4DT6H8M');
echo date_interval_format($interval, '%d days');

echo PHP_EOL;

$interval = new DateInterval('P32D');
echo date_interval_format($interval, '%d days');

?>
--EXPECTF--
4 days
32 days

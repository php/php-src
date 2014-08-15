--TEST--
Bug #50020
--INI--
date.timezone=GMT
--FILE--
<?php

try {
	$i = DateInterval::createFromDateString('P1D');
} catch (Exception $e) {
	var_dump($e->getMessage());
}
?>
--EXPECTF--
string(%d) "DateInterval::createFromDateString(): Failed to parse time string (P1D) at position 1 (1): Unexpected character"

--TEST--
Ini parsing errors should not result in memory leaks
--FILE--
<?php
var_dump(parse_ini_string('a="b'));
?>
--EXPECTF--
Warning: syntax error, unexpected $end, expecting TC_DOLLAR_CURLY or TC_QUOTED_STRING or '"' in Unknown on line 1
 in %s on line %d
bool(false)

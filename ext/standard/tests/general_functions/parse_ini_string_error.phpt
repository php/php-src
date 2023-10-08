--TEST--
Ini parsing errors should not result in memory leaks
--FILE--
<?php
var_dump(parse_ini_string('a="b'));
var_dump(parse_ini_string('a=${b'));
var_dump(parse_ini_string('a=${b:-a'));
?>
--EXPECTF--
Warning: syntax error, unexpected end of file, expecting TC_DOLLAR_CURLY or TC_QUOTED_STRING or '"' in Unknown on line 1
 in %s on line %d
bool(false)

Warning: syntax error, unexpected end of file, expecting TC_FALLBACK or '}' in Unknown on line 1
 in %s on line %d
bool(false)

Warning: syntax error, unexpected TC_FALLBACK, expecting TC_VARNAME in Unknown on line 1
 in %s on line %d
bool(false)

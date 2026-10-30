--TEST--
Delimiters crash test
--FILE--
<?php

var_dump(preg_match('', ''));
var_dump(preg_match('      ', ''));
var_dump(preg_match('@@', ''));
var_dump(preg_match('12', ''));
var_dump(preg_match('<>', ''));
var_dump(preg_match('~a', ''));
var_dump(preg_match('@\@\@@', '@@'));
var_dump(preg_match('//z', '@@'));
var_dump(preg_match('{', ''));
var_dump(preg_match("\0\0", ''));

?>
--EXPECTF--
Warning: preg_match(): Empty regular expression in %sdelimiters.php on line 3
bool(false)

Warning: preg_match(): Empty regular expression in %sdelimiters.php on line 4
bool(false)
int(1)

Warning: preg_match(): Delimiter must not be alphanumeric, backslash, or NUL byte in %sdelimiters.php on line 6
bool(false)
int(1)

Warning: preg_match(): No ending delimiter '~' found in %sdelimiters.php on line 8
bool(false)
int(1)

Warning: preg_match(): Unknown modifier 'z' in %sdelimiters.php on line 10
bool(false)

Warning: preg_match(): No ending matching delimiter '}' found in %sdelimiters.php on line 11
bool(false)

Warning: preg_match(): Delimiter must not be alphanumeric, backslash, or NUL byte in %sdelimiters.php on line 12
bool(false)

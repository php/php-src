--TEST--
Test escapeshellcmd() to escape the quotation
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == 'WIN' ) {
   die('skip...Invalid for Windows');
}
?>
--FILE--
<?php
echo "*** Testing escapeshellcmd() escape the quotation ***\n";
$data = array(
	'"abc',
	"'abc",
	'?<>',
	'()[]{}$',
	'%^',
	'#&;`|*?',
	'~<>\\',
	'%NOENV%',
	"abc' 'def",
	'abc" "def',
	"'abc def'",
	'"abc def"',
);

echo "case: default\n";

$count = 1;
foreach ($data AS $value) {
	echo "-- Test " . $count++ . " --\n";
	var_dump(escapeshellcmd($value));
}

echo "case: ESCAPE_CMD_PAIR\n";
$count = 1;
foreach ($data AS $value) {
	echo "-- Test " . $count++ . " --\n";
	var_dump(escapeshellcmd($value, ESCAPE_CMD_PAIR));
}

echo "case: ESCAPE_CMD_END\n";
$count = 1;
foreach ($data AS $value) {
	echo "-- Test " . $count++ . " --\n";
	var_dump(escapeshellcmd($value, ESCAPE_CMD_END));
}

echo "case: ESCAPE_CMD_ALL\n";
$count = 1;
foreach ($data AS $value) {
	echo "-- Test " . $count++ . " --\n";
	var_dump(escapeshellcmd($value, ESCAPE_CMD_ALL));
}

echo "Done\n";
?>
--EXPECTF--
*** Testing escapeshellcmd() escape the quotation ***
case: default
-- Test 1 --
string(5) "\"abc"
-- Test 2 --
string(5) "\'abc"
-- Test 3 --
string(6) "\?\<\>"
-- Test 4 --
string(14) "\(\)\[\]\{\}\$"
-- Test 5 --
string(3) "%\^"
-- Test 6 --
string(14) "\#\&\;\`\|\*\?"
-- Test 7 --
string(8) "\~\<\>\\"
-- Test 8 --
string(7) "%NOENV%"
-- Test 9 --
string(9) "abc' 'def"
-- Test 10 --
string(9) "abc" "def"
-- Test 11 --
string(9) "'abc def'"
-- Test 12 --
string(9) ""abc def""
case: ESCAPE_CMD_PAIR
-- Test 1 --
string(5) "\"abc"
-- Test 2 --
string(5) "\'abc"
-- Test 3 --
string(6) "\?\<\>"
-- Test 4 --
string(14) "\(\)\[\]\{\}\$"
-- Test 5 --
string(3) "%\^"
-- Test 6 --
string(14) "\#\&\;\`\|\*\?"
-- Test 7 --
string(8) "\~\<\>\\"
-- Test 8 --
string(7) "%NOENV%"
-- Test 9 --
string(9) "abc' 'def"
-- Test 10 --
string(9) "abc" "def"
-- Test 11 --
string(9) "'abc def'"
-- Test 12 --
string(9) ""abc def""
case: ESCAPE_CMD_END
-- Test 1 --
string(5) "\"abc"
-- Test 2 --
string(5) "\'abc"
-- Test 3 --
string(6) "\?\<\>"
-- Test 4 --
string(14) "\(\)\[\]\{\}\$"
-- Test 5 --
string(3) "%\^"
-- Test 6 --
string(14) "\#\&\;\`\|\*\?"
-- Test 7 --
string(8) "\~\<\>\\"
-- Test 8 --
string(7) "%NOENV%"
-- Test 9 --
string(11) "abc\' \'def"
-- Test 10 --
string(11) "abc\" \"def"
-- Test 11 --
string(9) "'abc def'"
-- Test 12 --
string(9) ""abc def""
case: ESCAPE_CMD_ALL
-- Test 1 --
string(5) "\"abc"
-- Test 2 --
string(5) "\'abc"
-- Test 3 --
string(6) "\?\<\>"
-- Test 4 --
string(14) "\(\)\[\]\{\}\$"
-- Test 5 --
string(3) "%\^"
-- Test 6 --
string(14) "\#\&\;\`\|\*\?"
-- Test 7 --
string(8) "\~\<\>\\"
-- Test 8 --
string(7) "%NOENV%"
-- Test 9 --
string(11) "abc\' \'def"
-- Test 10 --
string(11) "abc\" \"def"
-- Test 11 --
string(11) "\'abc def\'"
-- Test 12 --
string(11) "\"abc def\""
Done

--TEST--
Test escapeshellcmd() functionality on Windows
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != 'WIN' ) {
   die('skip...Valid for Windows only');
}
?>
--FILE--
<?php
echo "*** Testing escapeshellcmd() basic operations ***\n";
$data = array(
	'"abc',
	"'abc",
	'?<>',
	'()[]{}$',
	'%^',
	'#&;`|*?',
	'~<>\\',
	'%NOENV%'
);

$count = 1;
foreach ($data AS $value) {
	echo "-- Test " . $count++ . " --\n";
	var_dump(escapeshellcmd($value));
}

echo "Done\n";
?>
--EXPECTF--
*** Testing escapeshellcmd() basic operations ***
-- Test 1 --
string(5) "^"abc"
-- Test 2 --
string(5) "^'abc"
-- Test 3 --
string(6) "^?^<^>"
-- Test 4 --
string(14) "^(^)^[^]^{^}^$"
-- Test 5 --
string(4) "^%^^"
-- Test 6 --
string(14) "^#^&^;^`^|^*^?"
-- Test 7 --
string(8) "^~^<^>^\"
-- Test 8 --
string(9) "^%NOENV^%"
Done

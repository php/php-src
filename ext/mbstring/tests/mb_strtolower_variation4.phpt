--TEST--
Test mb_strtolower() function : usage variations - pass different characters to check conversion is correct
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strtolower') or die("skip mb_strtolower() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_strtolower(string $sourcestring [, string $encoding])
 * Description: Returns a lowercased version of $sourcestring
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass accented characters and Russian characters to check case conversion is correct
 */

echo "*** Testing mb_strtolower() :  usage variations ***\n";

$uppers = array('Basic Latin' => 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 
                'Characters With Accents' => base64_decode('w4DDgcOCw4PDhMOFw4bDh8OIw4nDisOLw4zDjcOOw4/DkMORw5LDk8OUw5XDlg=='),
                'Russian' => base64_decode('0JDQkdCS0JPQlNCV0JbQlw=='));
$lowers = array('Basic Latin' => 'abcdefghijklmnopqrstuvwxyz',
                'Characters With Accents' => base64_decode('w6DDocOiw6PDpMOlw6bDp8Oow6nDqsOrw6zDrcOuw6/DsMOxw7LDs8O0w7XDtg=='),
                'Russian' => base64_decode('0LDQsdCy0LPQtNC10LbQtw=='));

foreach ($uppers as $lang => $sourcestring) {
	echo "\n-- $lang --\n";
	$a = mb_strtolower($sourcestring, 'utf-8');
	var_dump(base64_encode($a));
	if ($a == $lowers[$lang]) {
		echo "Correctly Converted\n";
	} else {
		echo "Incorrectly Converted\n";
	}
}

echo "Done";
?>

--EXPECTF--
*** Testing mb_strtolower() :  usage variations ***

-- Basic Latin --
string(36) "YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXo="
Correctly Converted

-- Characters With Accents --
string(64) "w6DDocOiw6PDpMOlw6bDp8Oow6nDqsOrw6zDrcOuw6/DsMOxw7LDs8O0w7XDtg=="
Correctly Converted

-- Russian --
string(24) "0LDQsdCy0LPQtNC10LbQtw=="
Correctly Converted
Done
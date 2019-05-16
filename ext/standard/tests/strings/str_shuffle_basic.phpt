--TEST--
Test str_shuffle() function : basic functionality
--FILE--
<?php
/* Prototype  : string str_shuffle  ( string $str  )
 * Description: Randomly shuffles a string
 * Source code: ext/standard/string.c
*/

/*
 * Testing str_shuffle() : basic functionality
*/

echo "*** Testing str_shuffle() : basic functionality ***\n";

// Initialize all required variables
$str = 'This testcase tests the str_shuffle() function.';
var_dump(str_shuffle($str));


// For a given i/p string ensure that all combinations are
// generated given a reasonable sample of calls
$a = array();
$trys = 1000;
$ip = 'abcd';
$len_ip = strlen($ip);

for ($i = 0; $i < $trys; $i++) {
    $op = str_shuffle($ip);

    if (!is_string($op) || strlen($op) != $len_ip) {
    	echo "TEST FAILED\n";
    }

    // Combination already hit ?
    if (empty($a[$op])) {
    	// No first time init
     	$a[$op] = 0;
    }

    // Increment count for this combination
    $a[$op]++;
}

$combinations = count($a);

if ($combinations != 24) {
	echo "TEST FAILED.. Only $combinations out of a possible 24 combinations used\n";
} else {
	echo "TEST PASSED\n";
}

?>
===DONE===
--EXPECTF--
*** Testing str_shuffle() : basic functionality ***
string(47) "%s"
TEST PASSED
===DONE===

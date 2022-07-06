--TEST--
Test mb_strtoupper() function : usage varitations - Pass different character types to check conversion is correct
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strtoupper') or die("skip mb_strtoupper() is not available in this build");
?>
--FILE--
<?php
/*
 * Pass characters from different languages to check that mb_strtoupper is
 * doing a correct case conversion
 */

echo "*** Testing mb_strtoupper() : usage variations ***\n";

$uppers = array('Basic Latin' => 'ABCDEFGHIJKLMNOPQRSTUVWXYZ',
                'Characters With Accents' => base64_decode('w4DDgcOCw4PDhMOFw4bDh8OIw4nDisOLw4zDjcOOw4/DkMORw5LDk8OUw5XDlg=='),
                'Russian' => base64_decode('0JDQkdCS0JPQlNCV0JbQlw=='));
$lowers = array('Basic Latin' => 'abcdefghijklmnopqrstuvwxyz',
                'Characters With Accents' => base64_decode('w6DDocOiw6PDpMOlw6bDp8Oow6nDqsOrw6zDrcOuw6/DsMOxw7LDs8O0w7XDtg=='),
                'Russian' => base64_decode('0LDQsdCy0LPQtNC10LbQtw=='));

foreach ($lowers as $lang => $sourcestring) {
    echo "\n-- $lang --\n";
    $a = mb_strtoupper($sourcestring, 'UTF-8');
    var_dump(base64_encode($a));
    if ($a == $uppers[$lang]) {
        echo "Correctly Converted\n";
    } else {
        echo "Incorrectly Converted\n";
    }
}

echo "Done";
?>
--EXPECT--
*** Testing mb_strtoupper() : usage variations ***

-- Basic Latin --
string(36) "QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVo="
Correctly Converted

-- Characters With Accents --
string(64) "w4DDgcOCw4PDhMOFw4bDh8OIw4nDisOLw4zDjcOOw4/DkMORw5LDk8OUw5XDlg=="
Correctly Converted

-- Russian --
string(24) "0JDQkdCS0JPQlNCV0JbQlw=="
Correctly Converted
Done

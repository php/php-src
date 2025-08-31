--TEST--
Test mb_strtolower() function : usage variations - pass different characters to check conversion is correct
--EXTENSIONS--
mbstring
--FILE--
<?php
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
--EXPECT--
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

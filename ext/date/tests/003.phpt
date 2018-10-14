--TEST--
date suffixes test
--FILE--
<?php
date_default_timezone_set('UTC');

for ($i = 0; $i < 32; $i++) {
	var_dump(date("jS", mktime(0,0,0, 1, $i, 2006)));
}

echo "Done\n";
?>
--EXPECT--
string(4) "31st"
string(3) "1st"
string(3) "2nd"
string(3) "3rd"
string(3) "4th"
string(3) "5th"
string(3) "6th"
string(3) "7th"
string(3) "8th"
string(3) "9th"
string(4) "10th"
string(4) "11th"
string(4) "12th"
string(4) "13th"
string(4) "14th"
string(4) "15th"
string(4) "16th"
string(4) "17th"
string(4) "18th"
string(4) "19th"
string(4) "20th"
string(4) "21st"
string(4) "22nd"
string(4) "23rd"
string(4) "24th"
string(4) "25th"
string(4) "26th"
string(4) "27th"
string(4) "28th"
string(4) "29th"
string(4) "30th"
string(4) "31st"
Done

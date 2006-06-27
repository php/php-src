--TEST--
date suffixes test
--INI--
date.timezone=UTC
--FILE--
<?php

for ($i = 0; $i < 32; $i++) {
	var_dump(date("jS", mktime(0,0,0, 1, $i, 2006)));
}

echo "Done\n";
?>
--EXPECTF--	
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
--UEXPECTF--
unicode(4) "31st"
unicode(3) "1st"
unicode(3) "2nd"
unicode(3) "3rd"
unicode(3) "4th"
unicode(3) "5th"
unicode(3) "6th"
unicode(3) "7th"
unicode(3) "8th"
unicode(3) "9th"
unicode(4) "10th"
unicode(4) "11th"
unicode(4) "12th"
unicode(4) "13th"
unicode(4) "14th"
unicode(4) "15th"
unicode(4) "16th"
unicode(4) "17th"
unicode(4) "18th"
unicode(4) "19th"
unicode(4) "20th"
unicode(4) "21st"
unicode(4) "22nd"
unicode(4) "23rd"
unicode(4) "24th"
unicode(4) "25th"
unicode(4) "26th"
unicode(4) "27th"
unicode(4) "28th"
unicode(4) "29th"
unicode(4) "30th"
unicode(4) "31st"
Done

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

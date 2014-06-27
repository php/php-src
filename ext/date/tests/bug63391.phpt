--TEST--
Test for #63391 (Incorrect/inconsistent day of week prior to the year 1600)
--FILE--
<?php
ini_set('date.timezone', 'UTC');

print "Date         PHP\n";
print "----------   ---\n";
$dates = array('1599-12-30', '1599-12-31', '1600-01-01', '1600-01-02');
foreach ($dates as $date) {
	echo date_create($date)->format('Y-m-d   D'), "\n";
}
?>
--EXPECT--
Date         PHP
----------   ---
1599-12-30   Thu
1599-12-31   Fri
1600-01-01   Sat
1600-01-02   Sun

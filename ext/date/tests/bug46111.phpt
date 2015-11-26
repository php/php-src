--TEST--
Bug #46111 (strtotime() returns false for some valid timezones)
--FILE--
<?php
date_default_timezone_set('Asia/Calcutta');
$timezones = timezone_identifiers_list();

# An empty list indicates no errors
print "[strtotime(timezone) == false - Begin List]\n";
foreach ($timezones as $zone) {
    $date_string = "2008-01-01 13:00:00 " . $zone;
    
    if (!strtotime($date_string)) {
        echo $zone . "\n";
    }
}
print "[strtotime(timezone) == false - End List]\n";
?>
--EXPECT--
[strtotime(timezone) == false - Begin List]
[strtotime(timezone) == false - End List]

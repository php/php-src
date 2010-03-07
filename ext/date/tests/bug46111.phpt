--TEST--
Bug #46111 (strtotime() returns false for some valid timezones)
--FILE--
<?php
date_default_timezone_set('Asia/Calcutta');
$timezones = timezone_identifiers_list();

# An empty list indicates no errors, but in this case some of them can not be
# parsed. All of those are on the backwards compatible list though, so we'll
# just accept those failures here.
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
CST6CDT
Cuba
Egypt
Eire
EST5EDT
Etc/GMT0
Factory
GB
GB-Eire
GMT0
Greenwich
Hongkong
Iceland
Iran
Israel
Jamaica
Japan
Kwajalein
Libya
MST7MDT
Navajo
NZ
NZ-CHAT
Poland
Portugal
PRC
PST8PDT
ROC
ROK
Singapore
Turkey
Universal
US/Alaska
US/Aleutian
US/Arizona
US/Central
US/East-Indiana
US/Eastern
US/Hawaii
US/Indiana-Starke
US/Michigan
US/Mountain
US/Pacific
US/Pacific-New
US/Samoa
W-SU
Zulu
[strtotime(timezone) == false - End List]

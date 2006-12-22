--TEST--
timezone_abbreviations_list() tests
--INI--
date.timezone=UTC
--FILE--
<?php
  $timezone_abbreviations = timezone_abbreviations_list();
  var_dump($timezone_abbreviations["est"]);
  echo "Done\n";
?>
--EXPECTF--
array(70) {
  [0]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(16) "America/New_York"
  }
  [1]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Antigua"
  }
  [2]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(16) "America/Atikokan"
  }
  [3]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(21) "America/Cambridge_Bay"
  }
  [4]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(14) "America/Cancun"
  }
  [5]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(14) "America/Cayman"
  }
  [6]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Chicago"
  }
  [7]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(21) "America/Coral_Harbour"
  }
  [8]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Detroit"
  }
  [9]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(18) "America/Fort_Wayne"
  }
  [10]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(18) "America/Grand_Turk"
  }
  [11]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(28) "America/Indiana/Indianapolis"
  }
  [12]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(20) "America/Indiana/Knox"
  }
  [13]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(23) "America/Indiana/Marengo"
  }
  [14]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(26) "America/Indiana/Petersburg"
  }
  [15]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(21) "America/Indiana/Vevay"
  }
  [16]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(25) "America/Indiana/Vincennes"
  }
  [17]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(20) "America/Indianapolis"
  }
  [18]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Iqaluit"
  }
  [19]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Jamaica"
  }
  [20]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(27) "America/Kentucky/Louisville"
  }
  [21]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(27) "America/Kentucky/Monticello"
  }
  [22]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Knox_IN"
  }
  [23]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(18) "America/Louisville"
  }
  [24]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Managua"
  }
  [25]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(17) "America/Menominee"
  }
  [26]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(14) "America/Merida"
  }
  [27]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(16) "America/Montreal"
  }
  [28]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(14) "America/Nassau"
  }
  [29]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Nipigon"
  }
  [30]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(14) "America/Panama"
  }
  [31]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(19) "America/Pangnirtung"
  }
  [32]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(22) "America/Port-au-Prince"
  }
  [33]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(20) "America/Rankin_Inlet"
  }
  [34]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(21) "America/Santo_Domingo"
  }
  [35]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(19) "America/Thunder_Bay"
  }
  [36]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Toronto"
  }
  [37]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(14) "Canada/Eastern"
  }
  [38]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(3) "EST"
  }
  [39]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(7) "EST5EDT"
  }
  [40]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(7) "Jamaica"
  }
  [41]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(10) "US/Central"
  }
  [42]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "US/East-Indiana"
  }
  [43]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(10) "US/Eastern"
  }
  [44]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(17) "US/Indiana-Starke"
  }
  [45]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(11) "US/Michigan"
  }
  [46]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(13) "Australia/ACT"
  }
  [47]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(18) "Australia/Brisbane"
  }
  [48]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(18) "Australia/Canberra"
  }
  [49]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(16) "Australia/Currie"
  }
  [50]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(16) "Australia/Hobart"
  }
  [51]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(18) "Australia/Lindeman"
  }
  [52]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(19) "Australia/Melbourne"
  }
  [53]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(13) "Australia/NSW"
  }
  [54]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(20) "Australia/Queensland"
  }
  [55]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(16) "Australia/Sydney"
  }
  [56]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(18) "Australia/Tasmania"
  }
  [57]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(18) "Australia/Victoria"
  }
  [58]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(19) "Australia/Melbourne"
  }
  [59]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(13) "Australia/ACT"
  }
  [60]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(18) "Australia/Brisbane"
  }
  [61]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(18) "Australia/Canberra"
  }
  [62]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(16) "Australia/Currie"
  }
  [63]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(16) "Australia/Hobart"
  }
  [64]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(18) "Australia/Lindeman"
  }
  [65]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(13) "Australia/NSW"
  }
  [66]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(20) "Australia/Queensland"
  }
  [67]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(16) "Australia/Sydney"
  }
  [68]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(18) "Australia/Tasmania"
  }
  [69]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(18) "Australia/Victoria"
  }
}
Done

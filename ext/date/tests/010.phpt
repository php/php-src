--TEST--
timezone_abbreviations_list() tests
--FILE--
<?php
date_default_timezone_set('UTC');
  $timezone_abbreviations = timezone_abbreviations_list();
  var_dump($timezone_abbreviations["est"]);
  echo "Done\n";
?>
--EXPECTF--
array(71) {
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
    string(23) "America/Indiana/Winamac"
  }
  [18]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(20) "America/Indianapolis"
  }
  [19]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Iqaluit"
  }
  [20]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Jamaica"
  }
  [21]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(27) "America/Kentucky/Louisville"
  }
  [22]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(27) "America/Kentucky/Monticello"
  }
  [23]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Knox_IN"
  }
  [24]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(18) "America/Louisville"
  }
  [25]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Managua"
  }
  [26]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(17) "America/Menominee"
  }
  [27]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(14) "America/Merida"
  }
  [28]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(16) "America/Montreal"
  }
  [29]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(14) "America/Nassau"
  }
  [30]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Nipigon"
  }
  [31]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(14) "America/Panama"
  }
  [32]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(19) "America/Pangnirtung"
  }
  [33]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(22) "America/Port-au-Prince"
  }
  [34]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(20) "America/Rankin_Inlet"
  }
  [35]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(21) "America/Santo_Domingo"
  }
  [36]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(19) "America/Thunder_Bay"
  }
  [37]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "America/Toronto"
  }
  [38]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(14) "Canada/Eastern"
  }
  [39]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(3) "EST"
  }
  [40]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(7) "EST5EDT"
  }
  [41]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(7) "Jamaica"
  }
  [42]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(10) "US/Central"
  }
  [43]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(15) "US/East-Indiana"
  }
  [44]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(10) "US/Eastern"
  }
  [45]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(17) "US/Indiana-Starke"
  }
  [46]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(-18000)
    ["timezone_id"]=>
    string(11) "US/Michigan"
  }
  [47]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(13) "Australia/ACT"
  }
  [48]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(18) "Australia/Brisbane"
  }
  [49]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(18) "Australia/Canberra"
  }
  [50]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(16) "Australia/Currie"
  }
  [51]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(16) "Australia/Hobart"
  }
  [52]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(18) "Australia/Lindeman"
  }
  [53]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(19) "Australia/Melbourne"
  }
  [54]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(13) "Australia/NSW"
  }
  [55]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(20) "Australia/Queensland"
  }
  [56]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(16) "Australia/Sydney"
  }
  [57]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(18) "Australia/Tasmania"
  }
  [58]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(36000)
    ["timezone_id"]=>
    string(18) "Australia/Victoria"
  }
  [59]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(19) "Australia/Melbourne"
  }
  [60]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(13) "Australia/ACT"
  }
  [61]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(18) "Australia/Brisbane"
  }
  [62]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(18) "Australia/Canberra"
  }
  [63]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(16) "Australia/Currie"
  }
  [64]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(16) "Australia/Hobart"
  }
  [65]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(18) "Australia/Lindeman"
  }
  [66]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(13) "Australia/NSW"
  }
  [67]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(20) "Australia/Queensland"
  }
  [68]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(16) "Australia/Sydney"
  }
  [69]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(39600)
    ["timezone_id"]=>
    string(18) "Australia/Tasmania"
  }
  [70]=>
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

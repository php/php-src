--TEST--
timezone_abbreviations_list() tests
--FILE--
<?php
date_default_timezone_set('UTC');
  $timezone_abbreviations = timezone_abbreviations_list();
  var_dump($timezone_abbreviations["utc"]);
  echo "Done\n";
?>
--EXPECTF--
array(6) {
  [0]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(0)
    ["timezone_id"]=>
    string(16) "Antarctica/Troll"
  }
  [1]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(0)
    ["timezone_id"]=>
    string(13) "Etc/Universal"
  }
  [2]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(0)
    ["timezone_id"]=>
    string(7) "Etc/UTC"
  }
  [3]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(0)
    ["timezone_id"]=>
    string(8) "Etc/Zulu"
  }
  [4]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(0)
    ["timezone_id"]=>
    string(3) "UTC"
  }
  [5]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(0)
    ["timezone_id"]=>
    string(3) "UTC"
  }
}
Done

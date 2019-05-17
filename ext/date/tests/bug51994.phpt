--TEST--
Bug #51994 (date_parse_from_format is parsing invalid date using 'yz' format)
--FILE--
<?php
$trans_date = '10153'; // 152nd day of year 2010 -> 03.06.2010
$a_date	= date_parse_from_format('yz', $trans_date);
var_dump($a_date);
?>
--EXPECT--
array(12) {
  ["year"]=>
  int(2010)
  ["month"]=>
  int(6)
  ["day"]=>
  int(3)
  ["hour"]=>
  bool(false)
  ["minute"]=>
  bool(false)
  ["second"]=>
  bool(false)
  ["fraction"]=>
  bool(false)
  ["warning_count"]=>
  int(0)
  ["warnings"]=>
  array(0) {
  }
  ["error_count"]=>
  int(0)
  ["errors"]=>
  array(0) {
  }
  ["is_localtime"]=>
  bool(false)
}

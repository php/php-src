--TEST--
Bug #67253 (timelib_meridian_with_check out-of-bounds read)
--INI--
date.timezone=Europe/Berlin
--FILE--
<?php
$z = '';
var_dump(date_parse_from_format("aHa0", "0=G{$z}9UCNnF"));
?>
--EXPECT--
array(12) {
  ["year"]=>
  bool(false)
  ["month"]=>
  bool(false)
  ["day"]=>
  bool(false)
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
  int(3)
  ["errors"]=>
  array(2) {
    [0]=>
    string(29) "A meridian could not be found"
    [9]=>
    string(43) "Not enough data available to satisfy format"
  }
  ["is_localtime"]=>
  bool(false)
}

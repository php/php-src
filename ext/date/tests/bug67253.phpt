--TEST--
Bug #67253 (timelib_meridian_with_check out-of-bounds read)
--INI--
date.timezone=Europe/Berlin
--FILE--
<?php
$z = '';
var_dump(date_parse_from_format("aHa0", "0=G{$z}9UCNnF"));
--EXPECT--
array(12) {
  ["year"]=>
  bool(false)
  ["month"]=>
  bool(false)
  ["day"]=>
  bool(false)
  ["hour"]=>
  int(0)
  ["minute"]=>
  int(0)
  ["second"]=>
  int(0)
  ["fraction"]=>
  float(0)
  ["warning_count"]=>
  int(0)
  ["warnings"]=>
  array(0) {
  }
  ["error_count"]=>
  int(3)
  ["errors"]=>
  array(3) {
    [0]=>
    string(51) "Meridian can only come after an hour has been found"
    [1]=>
    string(29) "A meridian could not be found"
    [9]=>
    string(12) "Data missing"
  }
  ["is_localtime"]=>
  bool(false)
}

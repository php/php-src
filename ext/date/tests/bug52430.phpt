--TEST--
Bug #52430 (date_parse parse 24:xx:xx as valid time)
--FILE--
<?php
var_dump(date_parse('2010-1-1 24:59:59'));
?>
--EXPECT--
array(12) {
  ["year"]=>
  int(2010)
  ["month"]=>
  int(1)
  ["day"]=>
  int(1)
  ["hour"]=>
  int(24)
  ["minute"]=>
  int(59)
  ["second"]=>
  int(59)
  ["fraction"]=>
  float(0)
  ["warning_count"]=>
  int(1)
  ["warnings"]=>
  array(1) {
    [18]=>
    string(27) "The parsed time was invalid"
  }
  ["error_count"]=>
  int(0)
  ["errors"]=>
  array(0) {
  }
  ["is_localtime"]=>
  bool(false)
}

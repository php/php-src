--TEST--
Bug #67251 (date_parse_from_format out-of-bounds read)
--INI--
date.timezone=Europe/Berlin
--FILE--
<?php
var_dump(date_parse_from_format("\\","AAAABBBB"));
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
  int(2)
  ["errors"]=>
  array(1) {
    [0]=>
    string(13) "Trailing data"
  }
  ["is_localtime"]=>
  bool(false)
}

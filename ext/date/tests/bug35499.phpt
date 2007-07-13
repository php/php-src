--TEST--
Bug #35499 (strtotime() does not handle whitespace around the date string)
--FILE--
<?php
date_default_timezone_set("UTC");

echo date(DATE_ISO8601, strtotime("11/20/2005 8:00 AM \r\n")) . "\n";
echo date(DATE_ISO8601, strtotime("  11/20/2005 8:00 AM \r\n")) . "\n";
var_dump(date_parse(" a "));
var_dump(date_parse(" \n "));
?>
--EXPECT--
2005-11-20T08:00:00+0000
2005-11-20T08:00:00+0000
array(16) {
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
  int(0)
  ["errors"]=>
  array(0) {
  }
  ["is_localtime"]=>
  bool(true)
  ["zone_type"]=>
  int(2)
  ["zone"]=>
  int(-60)
  ["is_dst"]=>
  bool(false)
  ["tz_abbr"]=>
  string(1) "A"
}
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
  int(0)
  ["errors"]=>
  array(0) {
  }
  ["is_localtime"]=>
  bool(false)
}
--UEXPECT--
2005-11-20T08:00:00+0000
2005-11-20T08:00:00+0000
array(16) {
  [u"year"]=>
  bool(false)
  [u"month"]=>
  bool(false)
  [u"day"]=>
  bool(false)
  [u"hour"]=>
  bool(false)
  [u"minute"]=>
  bool(false)
  [u"second"]=>
  bool(false)
  [u"fraction"]=>
  bool(false)
  [u"warning_count"]=>
  int(0)
  [u"warnings"]=>
  array(0) {
  }
  [u"error_count"]=>
  int(0)
  [u"errors"]=>
  array(0) {
  }
  [u"is_localtime"]=>
  bool(true)
  [u"zone_type"]=>
  int(2)
  [u"zone"]=>
  int(-60)
  [u"is_dst"]=>
  bool(false)
  [u"tz_abbr"]=>
  unicode(1) "A"
}
array(12) {
  [u"year"]=>
  bool(false)
  [u"month"]=>
  bool(false)
  [u"day"]=>
  bool(false)
  [u"hour"]=>
  bool(false)
  [u"minute"]=>
  bool(false)
  [u"second"]=>
  bool(false)
  [u"fraction"]=>
  bool(false)
  [u"warning_count"]=>
  int(0)
  [u"warnings"]=>
  array(0) {
  }
  [u"error_count"]=>
  int(0)
  [u"errors"]=>
  array(0) {
  }
  [u"is_localtime"]=>
  bool(false)
}

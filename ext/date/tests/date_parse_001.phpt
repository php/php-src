--TEST--
Test basic date_parse()
--FILE--
<?php
    date_default_timezone_set('UTC');
    var_dump(date_parse("2006-12-12 10:00:00.5"));
    var_dump(date_parse("2006-12-12"));
    var_dump(date_parse("2006-12--12"));
    var_dump(date_parse("2006-02-30"));
    var_dump(date_parse("2006-03-04"));
    var_dump(date_parse("2006-03"));
    var_dump(date_parse("03-03"));
    var_dump(date_parse("0-0"));
    var_dump(date_parse(""));
    echo "Done\n";
?>
--EXPECT--
array(12) {
  ["year"]=>
  int(2006)
  ["month"]=>
  int(12)
  ["day"]=>
  int(12)
  ["hour"]=>
  int(10)
  ["minute"]=>
  int(0)
  ["second"]=>
  int(0)
  ["fraction"]=>
  float(0.5)
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
array(12) {
  ["year"]=>
  int(2006)
  ["month"]=>
  int(12)
  ["day"]=>
  int(12)
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
array(15) {
  ["year"]=>
  int(2006)
  ["month"]=>
  int(12)
  ["day"]=>
  int(1)
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
  int(1)
  ["errors"]=>
  array(1) {
    [7]=>
    string(20) "Unexpected character"
  }
  ["is_localtime"]=>
  bool(true)
  ["zone_type"]=>
  int(1)
  ["zone"]=>
  int(-43200)
  ["is_dst"]=>
  bool(false)
}
array(12) {
  ["year"]=>
  int(2006)
  ["month"]=>
  int(2)
  ["day"]=>
  int(30)
  ["hour"]=>
  bool(false)
  ["minute"]=>
  bool(false)
  ["second"]=>
  bool(false)
  ["fraction"]=>
  bool(false)
  ["warning_count"]=>
  int(1)
  ["warnings"]=>
  array(1) {
    [11]=>
    string(27) "The parsed date was invalid"
  }
  ["error_count"]=>
  int(0)
  ["errors"]=>
  array(0) {
  }
  ["is_localtime"]=>
  bool(false)
}
array(12) {
  ["year"]=>
  int(2006)
  ["month"]=>
  int(3)
  ["day"]=>
  int(4)
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
array(12) {
  ["year"]=>
  int(2006)
  ["month"]=>
  int(3)
  ["day"]=>
  int(1)
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
array(15) {
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
  array(2) {
    [0]=>
    string(20) "Unexpected character"
    [1]=>
    string(20) "Unexpected character"
  }
  ["is_localtime"]=>
  bool(true)
  ["zone_type"]=>
  int(1)
  ["zone"]=>
  int(-10800)
  ["is_dst"]=>
  bool(false)
}
array(15) {
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
  int(1)
  ["errors"]=>
  array(1) {
    [0]=>
    string(20) "Unexpected character"
  }
  ["is_localtime"]=>
  bool(true)
  ["zone_type"]=>
  int(1)
  ["zone"]=>
  int(0)
  ["is_dst"]=>
  bool(false)
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
  int(1)
  ["errors"]=>
  array(1) {
    [0]=>
    string(12) "Empty string"
  }
  ["is_localtime"]=>
  bool(false)
}
Done

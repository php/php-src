--TEST--
Test basic date_parse()
--INI--
date.timezone=UTC
--FILE--
<?php
    var_dump(date_parse("2006-12-12 10:00:00.5"));
	var_dump(date_parse("2006-12-12"));
    var_dump(date_parse("2006-12--12"));
    var_dump(date_parse("2006-02-30"));
    var_dump(date_parse("2006-03-04"));
    var_dump(date_parse("2006-03"));
    var_dump(date_parse("03-03"));
    var_dump(date_parse("0-0"));
    var_dump(date_parse(""));
    var_dump(date_parse(array()));
    echo "Done\n";
?>
--EXPECTF--
array(12) {
  [u"year"]=>
  int(2006)
  [u"month"]=>
  int(12)
  [u"day"]=>
  int(12)
  [u"hour"]=>
  int(10)
  [u"minute"]=>
  int(0)
  [u"second"]=>
  int(0)
  [u"fraction"]=>
  float(0.5)
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
array(12) {
  [u"year"]=>
  int(2006)
  [u"month"]=>
  int(12)
  [u"day"]=>
  int(12)
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
array(15) {
  [u"year"]=>
  int(2006)
  [u"month"]=>
  int(12)
  [u"day"]=>
  int(1)
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
  int(1)
  [u"errors"]=>
  array(1) {
    [7]=>
    unicode(20) "Unexpected character"
  }
  [u"is_localtime"]=>
  bool(true)
  [u"zone_type"]=>
  int(1)
  [u"zone"]=>
  int(720)
  [u"is_dst"]=>
  bool(false)
}
array(12) {
  [u"year"]=>
  int(2006)
  [u"month"]=>
  int(2)
  [u"day"]=>
  int(30)
  [u"hour"]=>
  bool(false)
  [u"minute"]=>
  bool(false)
  [u"second"]=>
  bool(false)
  [u"fraction"]=>
  bool(false)
  [u"warning_count"]=>
  int(1)
  [u"warnings"]=>
  array(1) {
    [11]=>
    unicode(27) "The parsed date was invalid"
  }
  [u"error_count"]=>
  int(0)
  [u"errors"]=>
  array(0) {
  }
  [u"is_localtime"]=>
  bool(false)
}
array(12) {
  [u"year"]=>
  int(2006)
  [u"month"]=>
  int(3)
  [u"day"]=>
  int(4)
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
array(12) {
  [u"year"]=>
  int(2006)
  [u"month"]=>
  int(3)
  [u"day"]=>
  int(1)
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
array(15) {
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
  int(2)
  [u"errors"]=>
  array(2) {
    [0]=>
    unicode(20) "Unexpected character"
    [1]=>
    unicode(20) "Unexpected character"
  }
  [u"is_localtime"]=>
  bool(true)
  [u"zone_type"]=>
  int(1)
  [u"zone"]=>
  int(180)
  [u"is_dst"]=>
  bool(false)
}
array(15) {
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
  int(1)
  [u"errors"]=>
  array(1) {
    [0]=>
    unicode(20) "Unexpected character"
  }
  [u"is_localtime"]=>
  bool(true)
  [u"zone_type"]=>
  int(1)
  [u"zone"]=>
  int(0)
  [u"is_dst"]=>
  bool(false)
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
  int(1)
  [u"errors"]=>
  array(1) {
    [0]=>
    unicode(12) "Empty string"
  }
  [u"is_localtime"]=>
  bool(false)
}

Warning: date_parse() expects parameter 1 to be binary string, array given in %sdate_parse_001.php on line 11
bool(false)
Done

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
    [7]=>
    string(20) "Unexpected character"
  }
  ["is_localtime"]=>
  bool(true)
  ["zone_type"]=>
  int(1)
  ["zone"]=>
  int(720)
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
  int(180)
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

Warning: date_parse() expects parameter 1 to be string, array given in %s on line %d
bool(false)
Done
--UEXPECTF--
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
    [7]=>
    string(20) "Unexpected character"
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
    string(20) "Unexpected character"
    [1]=>
    string(20) "Unexpected character"
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
    string(20) "Unexpected character"
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
    string(12) "Empty string"
  }
  [u"is_localtime"]=>
  bool(false)
}

Warning: date_parse() expects parameter 1 to be binary string, array given in %s on line %d
bool(false)
Done

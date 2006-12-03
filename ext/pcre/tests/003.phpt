--TEST--
abusing preg_match_all()
--FILE--
<?php

foreach (array(PREG_PATTERN_ORDER, PREG_SET_ORDER) as $flag) {
	var_dump(preg_match_all('~
		(?P<date> 
		(?P<year>(\d{2})?\d\d) -
		(?P<month>(?:\d\d|[a-zA-Z]{2,3})) -
		(?P<day>[0-3]?\d))
		~x',
		'2006-05-13 e outra data: "12-Aug-37"', $m, $flag));

	var_dump($m);
}
?>
--EXPECT--
int(2)
array(10) {
  [0]=>
  array(2) {
    [0]=>
    string(10) "2006-05-13"
    [1]=>
    string(9) "12-Aug-37"
  }
  ["date"]=>
  array(2) {
    [0]=>
    string(10) "2006-05-13"
    [1]=>
    string(9) "12-Aug-37"
  }
  [1]=>
  array(2) {
    [0]=>
    string(10) "2006-05-13"
    [1]=>
    string(9) "12-Aug-37"
  }
  ["year"]=>
  array(2) {
    [0]=>
    string(4) "2006"
    [1]=>
    string(2) "12"
  }
  [2]=>
  array(2) {
    [0]=>
    string(4) "2006"
    [1]=>
    string(2) "12"
  }
  [3]=>
  array(2) {
    [0]=>
    string(2) "20"
    [1]=>
    string(0) ""
  }
  ["month"]=>
  array(2) {
    [0]=>
    string(2) "05"
    [1]=>
    string(3) "Aug"
  }
  [4]=>
  array(2) {
    [0]=>
    string(2) "05"
    [1]=>
    string(3) "Aug"
  }
  ["day"]=>
  array(2) {
    [0]=>
    string(2) "13"
    [1]=>
    string(2) "37"
  }
  [5]=>
  array(2) {
    [0]=>
    string(2) "13"
    [1]=>
    string(2) "37"
  }
}
int(2)
array(2) {
  [0]=>
  array(10) {
    [0]=>
    string(10) "2006-05-13"
    ["date"]=>
    string(10) "2006-05-13"
    [1]=>
    string(10) "2006-05-13"
    ["year"]=>
    string(4) "2006"
    [2]=>
    string(4) "2006"
    [3]=>
    string(2) "20"
    ["month"]=>
    string(2) "05"
    [4]=>
    string(2) "05"
    ["day"]=>
    string(2) "13"
    [5]=>
    string(2) "13"
  }
  [1]=>
  array(10) {
    [0]=>
    string(9) "12-Aug-37"
    ["date"]=>
    string(9) "12-Aug-37"
    [1]=>
    string(9) "12-Aug-37"
    ["year"]=>
    string(2) "12"
    [2]=>
    string(2) "12"
    [3]=>
    string(0) ""
    ["month"]=>
    string(3) "Aug"
    [4]=>
    string(3) "Aug"
    ["day"]=>
    string(2) "37"
    [5]=>
    string(2) "37"
  }
}
--UEXPECT--
int(2)
array(10) {
  [0]=>
  array(2) {
    [0]=>
    unicode(10) "2006-05-13"
    [1]=>
    unicode(9) "12-Aug-37"
  }
  [u"date"]=>
  array(2) {
    [0]=>
    unicode(10) "2006-05-13"
    [1]=>
    unicode(9) "12-Aug-37"
  }
  [1]=>
  array(2) {
    [0]=>
    unicode(10) "2006-05-13"
    [1]=>
    unicode(9) "12-Aug-37"
  }
  [u"year"]=>
  array(2) {
    [0]=>
    unicode(4) "2006"
    [1]=>
    unicode(2) "12"
  }
  [2]=>
  array(2) {
    [0]=>
    unicode(4) "2006"
    [1]=>
    unicode(2) "12"
  }
  [3]=>
  array(2) {
    [0]=>
    unicode(2) "20"
    [1]=>
    unicode(0) ""
  }
  [u"month"]=>
  array(2) {
    [0]=>
    unicode(2) "05"
    [1]=>
    unicode(3) "Aug"
  }
  [4]=>
  array(2) {
    [0]=>
    unicode(2) "05"
    [1]=>
    unicode(3) "Aug"
  }
  [u"day"]=>
  array(2) {
    [0]=>
    unicode(2) "13"
    [1]=>
    unicode(2) "37"
  }
  [5]=>
  array(2) {
    [0]=>
    unicode(2) "13"
    [1]=>
    unicode(2) "37"
  }
}
int(2)
array(2) {
  [0]=>
  array(10) {
    [0]=>
    unicode(10) "2006-05-13"
    [u"date"]=>
    unicode(10) "2006-05-13"
    [1]=>
    unicode(10) "2006-05-13"
    [u"year"]=>
    unicode(4) "2006"
    [2]=>
    unicode(4) "2006"
    [3]=>
    unicode(2) "20"
    [u"month"]=>
    unicode(2) "05"
    [4]=>
    unicode(2) "05"
    [u"day"]=>
    unicode(2) "13"
    [5]=>
    unicode(2) "13"
  }
  [1]=>
  array(10) {
    [0]=>
    unicode(9) "12-Aug-37"
    [u"date"]=>
    unicode(9) "12-Aug-37"
    [1]=>
    unicode(9) "12-Aug-37"
    [u"year"]=>
    unicode(2) "12"
    [2]=>
    unicode(2) "12"
    [3]=>
    unicode(0) ""
    [u"month"]=>
    unicode(3) "Aug"
    [4]=>
    unicode(3) "Aug"
    [u"day"]=>
    unicode(2) "37"
    [5]=>
    unicode(2) "37"
  }
}

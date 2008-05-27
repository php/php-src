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

--TEST--
abusing preg_match()
--FILE--
<?php

foreach (array('2006-05-13', '06-12-12', 'data: "12-Aug-87"') as $s) {
	var_dump(preg_match('~
		(?P<date> 
		(?P<year>(\d{2})?\d\d) -
		(?P<month>(?:\d\d|[a-zA-Z]{2,3})) -
		(?P<day>[0-3]?\d))
	~x', $s, $m));

	var_dump($m);
}

?>
--EXPECT--
int(1)
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
int(1)
array(10) {
  [0]=>
  unicode(8) "06-12-12"
  [u"date"]=>
  unicode(8) "06-12-12"
  [1]=>
  unicode(8) "06-12-12"
  [u"year"]=>
  unicode(2) "06"
  [2]=>
  unicode(2) "06"
  [3]=>
  unicode(0) ""
  [u"month"]=>
  unicode(2) "12"
  [4]=>
  unicode(2) "12"
  [u"day"]=>
  unicode(2) "12"
  [5]=>
  unicode(2) "12"
}
int(1)
array(10) {
  [0]=>
  unicode(8) "12-Aug-8"
  [u"date"]=>
  unicode(8) "12-Aug-8"
  [1]=>
  unicode(8) "12-Aug-8"
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
  unicode(1) "8"
  [5]=>
  unicode(1) "8"
}

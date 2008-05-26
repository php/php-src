--TEST--
Test natsort and natcasesort
--INI--
precision=14
--FILE--
<?php
$data = array(
	'Test1',
	'teST2'=>0,
	5=>'test2',
	'abc'=>'test10',
	'test21'
);

var_dump($data);

natsort($data);
var_dump($data);

natcasesort($data);
var_dump($data);
?>
--EXPECT--
array(5) {
  [0]=>
  unicode(5) "Test1"
  [u"teST2"]=>
  int(0)
  [5]=>
  unicode(5) "test2"
  [u"abc"]=>
  unicode(6) "test10"
  [6]=>
  unicode(6) "test21"
}
array(5) {
  [u"teST2"]=>
  int(0)
  [0]=>
  unicode(5) "Test1"
  [5]=>
  unicode(5) "test2"
  [u"abc"]=>
  unicode(6) "test10"
  [6]=>
  unicode(6) "test21"
}
array(5) {
  [u"teST2"]=>
  int(0)
  [0]=>
  unicode(5) "Test1"
  [5]=>
  unicode(5) "test2"
  [u"abc"]=>
  unicode(6) "test10"
  [6]=>
  unicode(6) "test21"
}

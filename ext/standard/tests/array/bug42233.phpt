--TEST--
Bug #42233 (extract(): scandic characters not allowed as variable name)
--FILE--
<?php

$test = array(
	'a'     => '1',
	'æ'     => '2',
	'æøåäö' => '3',
);

var_dump($test);
var_dump(extract($test));
var_dump($a);
var_dump($æ);
var_dump($æøåäö);

echo "Done.\n";
?>
--EXPECT--
array(3) {
  [u"a"]=>
  unicode(1) "1"
  [u"æ"]=>
  unicode(1) "2"
  [u"æøåäö"]=>
  unicode(1) "3"
}
int(3)
unicode(1) "1"
unicode(1) "2"
unicode(1) "3"
Done.

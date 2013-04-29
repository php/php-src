--TEST--
Symfony StubNumberFormatterTest#testFormatTypeInt64Intl #3
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php


// PHP Unit's code to unserialize data passed as args to #testFormatTypeInt64Intl
$unit_test_args = unserialize('a:3:{i:0;O:15:"NumberFormatter":0:{}i:1;d:2147483648;i:2;s:13:"2,147,483,648";}');

var_dump($unit_test_args);

// execute the code from #testFormatTypeInt64Intl
$unit_test_args[0]->format($unit_test_args[1], \NumberFormatter::TYPE_INT64);

echo "== didn't crash ==".PHP_EOL;

?>
--EXPECT--
array(3) {
  [0]=>
  object(NumberFormatter)#1 (0) {
  }
  [1]=>
  float(2147483648)
  [2]=>
  string(13) "2,147,483,648"
}
== didn't crash ==

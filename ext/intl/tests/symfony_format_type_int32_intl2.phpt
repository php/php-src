--TEST--
Symfony StubNumberFormatterTest#testFormatTypeInt32Intl #2
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

// StubNumberFormatterTest#testFormatTypeInt32Intl is tested many times, each with different args.
// there are 7 sets of args that crash PHP (and other args that don't), each of those 7 is now a separate PHPT test
// to ensure that each of the 7 args are always tested.

// PHP Unit's code to unserialize data passed as args to #testFormatTypeInt32Intl
$unit_test_args = unserialize('a:3:{i:0;O:15:"NumberFormatter":0:{}i:1;d:1.1000000000000001;i:2;s:1:"1";}');

var_dump($unit_test_args);

// execute the code from #testFormatTypeInt32Intl
$unit_test_args[0]->format($unit_test_args[1], \NumberFormatter::TYPE_INT32);

echo "== didn't crash ==".PHP_EOL;

?>
--EXPECT--
array(3) {
  [0]=>
  object(NumberFormatter)#1 (0) {
  }
  [1]=>
  float(1.1)
  [2]=>
  string(1) "1"
}
== didn't crash ==

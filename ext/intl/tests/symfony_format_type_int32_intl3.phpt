--TEST--
Symfony StubNumberFormatterTest#testFormatTypeInt32Intl #3
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php


// PHP Unit's code to unserialize data passed as args to #testFormatTypeInt32Intl
$unit_test_args = unserialize('a:4:{i:0;O:15:"NumberFormatter":0:{}i:1;d:2147483648;i:2;s:14:"-2,147,483,648";i:3;s:83:"->format() TYPE_INT32 formats inconsistently an integer if out of the 32 bit range.";}');

var_dump($unit_test_args);

// execute the code from #testFormatTypeInt32Intl
try {
    $unit_test_args[0]->format($unit_test_args[1], \NumberFormatter::TYPE_INT32);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}
--EXPECT--
array(4) {
  [0]=>
  object(NumberFormatter)#1 (0) {
  }
  [1]=>
  float(2147483648)
  [2]=>
  string(14) "-2,147,483,648"
  [3]=>
  string(83) "->format() TYPE_INT32 formats inconsistently an integer if out of the 32 bit range."
}
Found unconstructed NumberFormatter

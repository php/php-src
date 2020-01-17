--TEST--
Symfony StubNumberFormatterTest#testFormatTypeInt32Intl #4
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php


// PHP Unit's code to unserialize data passed as args to #testFormatTypeInt32Intl
$unit_test_args = unserialize('a:3:{i:0;O:15:"NumberFormatter":0:{}i:1;i:1;i:2;s:7:"SFD1.00";}');

var_dump($unit_test_args);

// execute the code from #testFormatTypeInt32Intl
try {
    $unit_test_args[0]->format($unit_test_args[1], \NumberFormatter::TYPE_INT32);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}
--EXPECT--
array(3) {
  [0]=>
  object(NumberFormatter)#1 (0) {
  }
  [1]=>
  int(1)
  [2]=>
  string(7) "SFD1.00"
}
Found unconstructed NumberFormatter

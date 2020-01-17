--TEST--
Symfony StubNumberFormatterTest#testFormatTypeDoubleIntl #4
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php


// PHP Unit's code to unserialize data passed as args to #testFormatTypeDoubleIntl
$unit_test_args = unserialize('a:3:{i:0;O:15:"NumberFormatter":0:{}i:1;d:1.1000000000000001;i:2;s:7:"SFD1.10";}');

var_dump($unit_test_args);

// execute the code from #testFormatTypeDoubleIntl
try {
    $unit_test_args[0]->format($unit_test_args[1], \NumberFormatter::TYPE_DOUBLE);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
array(3) {
  [0]=>
  object(NumberFormatter)#1 (0) {
  }
  [1]=>
  float(1.1)
  [2]=>
  string(7) "SFD1.10"
}
Found unconstructed NumberFormatter

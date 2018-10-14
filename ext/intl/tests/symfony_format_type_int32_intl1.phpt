--TEST--
Symfony StubNumberFormatterTest#testFormatTypeInt32Intl #1
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

// port of Symfony's Symfony\Component\Locale\Tests\Stub\StubNumberFormatterTest#testFormatTypeInt32Intl


// Crashes on Windows
// Windows note: the popup '...program has stopped working'(AEDebug Popup)
//    doesn't always show if you're rapidly running this test repeatedly.
//    regardless of that, the test always crashes every time.
//    (it will show up the first time, or if you wait a while before running it again.)
//    (the popup may also be disabled, which can be done with a registry setting.)
//    you can confirm it crashed by checking the exit code OR
//    the message this test prints at the very end (expected output for pass).
//
// Get Exit Code
//  Linux: echo $?
//  Windows: echo %ErrorLevel%





// PHP Unit's code to unserialize data passed as args to #testFormatTypeInt32Intl
$unit_test_args = unserialize('a:3:{i:0;O:15:"NumberFormatter":0:{}i:1;i:1;i:2;s:1:"1";}');

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
  int(1)
  [2]=>
  string(1) "1"
}
== didn't crash ==

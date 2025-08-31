--TEST--
Test ob_start() function : closures as output handlers
--INI--
output_buffering=0
--FILE--
<?php
echo "*** Testing ob_start() : closures as output handlers ***\n";

ob_start(function ($output) {
  return 'Output (1): ' . $output;
});

ob_start(function ($output) {
  return 'Output (2): ' . $output;
});

echo "Test\nWith newlines\n";

$str1 = ob_get_contents ();

ob_end_flush();

$str2 = ob_get_contents ();

ob_end_flush();

echo $str1, $str2;

?>
--EXPECT--
*** Testing ob_start() : closures as output handlers ***
Output (1): Output (2): Test
With newlines
Test
With newlines
Output (2): Test
With newlines

<?php
 /*
  Helper for simple tests to check return-value. Usage:

  $tests = <<<TESTS
   expected_return_value === expression
   2                     === 1+1
   4                     === 2*2
   FALSE                 === @ fopen('non_existent_file')
TESTS;
  include( '../../../(../)tests/quicktester.inc' );

  Expect: OK

  Remember to NOT put a trailing ; after a line!

 */
 error_reporting(E_ALL);  
 $tests = explode("\n",$tests);
 $success = TRUE;
 foreach ($tests as $test)
 {
	 $res = eval("return ($test);");
	 $success = $success && $res;
	 if (!$res)
	 {
		 echo "\nAssert failed:\n";
		 echo "$test\n";
		 list($left,$right) = explode('===',$test);
		 echo "Left:  ";var_dump(eval("return ($left );"));
		 echo "Right: ";var_dump(eval("return ($right);"));
	 }
 }
 if ($success) echo "OK";
		 

<?php
 error_reporting(0);  
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
		 

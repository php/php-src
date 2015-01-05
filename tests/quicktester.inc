<?php
 /*
  Helper for simple tests to check return-value. Usage:

  $tests = <<<TESTS
   expected_return_value === expression
   2                     === 1+1
   4                     === 2*2
   FALSE                 === @ fopen('non_existent_file')
TESTS;
  include( 'tests/quicktester.inc' );

  Expect: OK

  Remember to NOT put a trailing ; after a line!

 */
error_reporting(E_ALL);  
$tests = explode("\n",$tests);
$success = TRUE;
foreach ($tests as $n=>$test)
{
	// ignore empty lines
	if (!$test) continue;

	// warn for trailing ;
	if (substr(trim($test), -1, 1) === ';') {
		echo "WARNING: trailing ';' found in test ".($n+1)."\n";
		exit;
	}

	// try for operators
	$operators = array('===', '~==');
	$operator = NULL;
	foreach ($operators as $a_operator) {
		if (strpos($test, $a_operator)!== FALSE) {
			$operator = $a_operator;
			list($left,$right) = explode($operator, $test);
			break;
		}
	}
	if (!$operator) {
		echo "WARNING: unknown operator in '$test' (1)\n";
		exit;
	}

	$left  = eval("return ($left );");
	$right = eval("return ($right);");
	switch (@$operator) {
		case '===': // exact match
			$result = $left === $right;
			break;
		case '~==': // may differ after 12th significant number
			if (   !is_float($left ) && !is_int($left )
				|| !is_float($right) && !is_int($right)) {
				$result = FALSE;
				break;
			}
			$result = abs(($left-$right) / $left) < 1e-12;
			break;
		default:
			echo "WARNING: unknown operator in '$test' (2)\n";
			exit;
	}

	$success = $success && $result;
	if (!$result) {
		echo "\nAssert failed:\n";
		echo "$test\n";
		echo "Left:  ";var_dump($left );
		echo "Right: ";var_dump($right);
	}
}
if ($success) echo "OK";
		 

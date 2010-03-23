<?php
/*
 * Common definition and Settings
 */

// Custom Error Hanlder for testing
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	global $debug;
	
	$err_type = array (
				1	=> "Error",			// E_ERROR
				2	=> "Warning",			// E_WARINING
				4	=> "Parsing Error",	// E_PARSE
				8	=> "Notice",			// E_NOTICE
				16	=> "Core Error",		// E_CORE_ERROR
				32	=> "Core Warning",		// E_CORE_WARNING
				64	=> "Compile Error",	// E_COMPILE_ERROR
				128 => "Compile Warning",	// E_COMPILE_WARNING
				256 => "User Error",		// E_USER_ERROR
				512 => "User Warning",		// E_USER_WARMING
				1024=> "User Notice",		// E_USER_NOTICE
				2048=> "Strict Notice",      // E_STRICT
				4096=> "Catchable fatal error",      // E_RECOVERABLE_ERROR
				);
	
	if (!empty($debug)) {
		printf("%s: %s (%d)\n", $err_type[$err_no], $err_msg, $linenum);
	}
	else {
		printf("ERR: %s\n",$err_type[$err_no]);
	}
}

set_error_handler('test_error_handler');


// Var def for testing
$t_ary = array(
		's1' => '日本語EUC-JPの文字列',
		's2' => 'English Text'
		);

class tc
{
	public $s1 = '日本語EUC-JPの文字列';
	public $s2 = 'English Text';

	function tc() 
	{
	}
}

$t_obj = new tc;

?>

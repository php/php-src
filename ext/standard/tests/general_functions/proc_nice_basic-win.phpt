--TEST--
proc_nice() basic behaviour
--SKIPIF--
<?php
/* No function_exists() check, proc_nice() is always available on Windows */

if (!defined('PHP_WINDOWS_VERSION_MAJOR')) {
	die('skip: Only for Windows');
}

if (PHP_SAPI != 'cli') {
	die('skip: Only for CLI');
}

if (getenv('SKIP_SLOW_TESTS')) {
	doe('skip: Slow test');
}
?>
--FILE--
<?php
function get_priority_from_wmic() {
	static $bin, $pid;

	if (!$bin) {
		$t 	= explode('\\', PHP_BINARY);

		$bin	= end($t);
		$pid	= getmypid();
	}

	$t = '';
	$p = popen('wmic process where name="' . $bin . '"', 'r');

	if (!$p) {
		return false;
	}

	while(!feof($p)) {
		$t .= fread($p, 1024);
	}

	pclose($p);

	$t = explode(PHP_EOL, $t);

	$f = false;
	$m = [
		strpos($t[0], ' ProcessId' ), 
		strpos($t[0], ' Priority ')
		];

	foreach ($t as $n => $l) {
		if (!$n || empty($l)) {
			continue;
		}

		$d = [];

		foreach ($m as $c) {
			$d[] = (int) substr($l, $c + 1, strpos($l, ' ', $c + 2) - ($c + 1));
		}

		if ($d[0] === $pid) {
			return $d[1];
		}
	}

	return false;
}

$p = [
	/* '<verbose name>' => ['<wmic value>', '<proc_nice value>'] */

	'Idle' 		=> [6, -10], 
	'Below normal'	=> [4, -3], 
	'Normal'	=> [8, 0], 
	'Above normal'	=> [10, 4], 
	'High priority'	=> [13, 5], 
	'Real time'	=> [24, 8]
	];

foreach ($p as $test => $data) {
	printf('Testing \'%s\' (%d): ', $test, $data[1]);

	proc_nice($data[1]);

	print (get_priority_from_wmic() === $data[0] ? 'Passed' : 'Failed') . PHP_EOL;
}
?>
--EXPECTF--
Testing 'Idle' (-10): Passed
Testing 'Below normal' (-3): Passed
Testing 'Normal' (0): Passed
Testing 'Above normal' (4): Passed
Testing 'High priority' (5): Passed
Testing 'Real time' (8): Passed

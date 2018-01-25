--TEST--
Bug #64770 stream_select() fails with pipes from proc_open()
--FILE--
<?php

$descs = array(
	0 => array('pipe', 'r'), // stdin
	1 => array('pipe', 'w'), // stdout
	2 => array('pipe', 'w'), // strerr
);

$other_opts = array('suppress_errors' => false, 'binary_pipes' => true);

$cmd = (substr(PHP_OS, 0, 3) == 'WIN') ? 'dir' : 'ls';
$p = proc_open($cmd, $descs, $pipes, '.', NULL, $other_opts);

if (is_resource($p)) {
	$data = '';

	while (1) {
		$w = $e = NULL;
		$n = stream_select($pipes, $w, $e, 300);

		if ($n === false) {
			echo "no streams \n";
			break;
		} else if ($n === 0) {
			echo "process timed out\n";
			proc_terminate($p, 9);
			break;
		} else if ($n > 0) {
			$line = fread($pipes[1], 8192);
			if (strlen($line) == 0) {
				/* EOF */
				break;
			}
			$data .= $line;
		}
	}
	var_dump(strlen($data));

	$ret = proc_close($p);
	var_dump($ret);
} else {
	echo "no process\n";
}
?>
==DONE==
--EXPECTF--
int(%d)
int(0)
==DONE==

<?php
	if (!extension_loaded('dba')) die('skip dba extension not available');
	if (!function_exists('dba_handlers')) die ('skip dba_handlers() not available');
	if (!sizeof(dba_handlers())) die('skip no handlers installed');
	if (!isset($handler)) {
		$handlers = dba_handlers();
		if (in_array('flatfile', $handlers)) {
			$handler = 'flatfile';
		} else {
			$handler = array_diff($handler, array('cdb', 'cdb_make')); /* these can only read OR write */
			if (count($handler)==0) {
				die('skip no handler available that can be used for the test');
			}
			$handler = array_shift($handler);
		}
	} else {
		if (!in_array($handler, dba_handlers())) {
			$HND = strtoupper($handler);
			die("skip $HND handler not available");
		}
	}
	$HND = strtoupper($handler);
?>

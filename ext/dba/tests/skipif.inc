<?php
	if (!extension_loaded('dba')) die('skip dba extension not available');
	if (!function_exists('dba_handlers')) die ('skip dba_handlers() not available');
	if (!sizeof(dba_handlers())) die('skip no handlers installed');
	if (!isset($handler)) {
		$handlers = dba_handlers();
		$handler  = false;
		$acceptable = array('flatfile','db4','db3','db2','gdbm','ndbm','dbm','inifile');
		foreach($acceptable as $hnd) {
			if (in_array($hnd, $handlers)) {
				$handler = $hnd;
				break;
			}
		}
		if ($handler === false) {
			die('skip No acceptable handler found');
		}
	} else {
		if (!in_array($handler, dba_handlers())) {
			$HND = strtoupper($handler);
			die("skip $HND handler not available");
		}
	}
	$HND = strtoupper($handler);
?>

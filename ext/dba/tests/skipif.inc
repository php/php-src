<?php
	if (!extension_loaded('dba')) die('skip dba extension not available');
	if (!function_exists('dba_handlers')) die ('skip dba_handlers() not available');
	if (!sizeof(dba_handlers())) die('skip no handlers installed');
	$handler = dba_handlers(); 
	if (in_array('flatfile', $handler)) {
		$handler = 'flatfile';
	} elseif ($handler[0]=='cdb') { // CDB currently supports only reading 
		if (count($handler)==1) {
			die('skip CDB currently supports only reading');
		}
		if ($handler[1]=='cdb_make' && count($handler)==2) {
			die('skip CDB currently supports only reading and creating');
		}
		$handler = $handler[1];
	} else {
		$handler = $handler[0];
	}
	$HND = strtoupper($handler);
?>

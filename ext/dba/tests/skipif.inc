<?php
	if (!extension_loaded('dba')) die('skip dba extension not available');
	if (!function_exists('dba_handlers')) die ('skip dba_handlers() not available');
	if (!sizeof(dba_handlers())) die('skip no handlers installed');
	// CDB currently supports only reading 
	$handler = dba_handlers(); 
	if ($handler[0]=='cdb') {
		if (count($handler)==1) {
			die('skip CDB currently supports only reading');
		}
		if ($handler[1]=='cdb_make' && count($handler)==2) {
			die('skip CDB currently supports only reading and creating');
		}
	}
	$HND = strtoupper($handler[0]);
?>

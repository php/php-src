<?php
	$db_filename = $db_file = dirname(__FILE__).'/test0.dbm';
	$handler = dba_handlers(); 
	// CDB currently supports only reading 
	if ($handler[0]=='cdb') {
		if (count($handler)==1) {
			die('CDB currently supports only reading ');
		}
		$handler = $handler[1];
	} else {
		$handler = $handler[0];
	}
	$lock_flag = 'l';
	@unlink($db_filename);
	@unlink($db_filename.'.lck');
?>

<?php
	$db_filename = $db_file = dirname(__FILE__).'/test0.dbm';
	$handler = dba_handlers(); 
	if (in_array('flatfile', $handler)) {
		$handler = 'flatfile';
	} elseif ($handler[0]=='cdb') { // CDB currently supports only reading 
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

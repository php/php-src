<?php
	require_once('skipif.inc');
	$lock_flag = 'l';
	$db_filename = $db_file = dirname(__FILE__).'/test0.dbm';
	@unlink($db_filename);
	@unlink($db_filename.'.lck');
?>

<?php
    $db_filename = dirname(__FILE__) .'/test0.dbm';  // see test.inc
	@unlink($db_filename);
	@unlink($db_filename.'.lck');
?>

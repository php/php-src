--TEST--
bug #34810 (mysqli::init() and others use wrong $this pointer without checks)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

class DbConnection { 
	public function connect() {
		include "connect.inc";

		$link = mysqli::connect($host, $user, $passwd);
		var_dump($link); 
		
		$link = mysqli::init();
		var_dump($link);
	} 
} 

$db = new DbConnection(); 
$db->connect();

echo "Done\n";
?>
--EXPECTF--	
object(mysqli)#%d (0) {
}
object(mysqli)#%d (0) {
}
Done

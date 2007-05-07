--TEST--
Bug #34810 (mysqli::init() and others use wrong $this pointer without checks)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

class DbConnection { 
	public function connect() {
		include "connect.inc";

		$link = mysqli_connect($host, $user, $passwd);
		var_dump($link); 
		
		$link = mysqli_init();
		var_dump($link);
		
		$mysql = new mysqli($host, $user, $passwd, "test");
		$mysql->query("DROP TABLE IF EXISTS test_warnings");
		$mysql->query("CREATE TABLE test_warnings (a int not null)");
		$mysql->query("SET sql_mode=''");
		$mysql->query("INSERT INTO test_warnings VALUES (1),(2),(NULL)");
		var_dump(mysqli_warning::__construct($mysql));
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
object(mysqli_warning)#%d (0) {
}
Done

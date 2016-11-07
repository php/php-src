--TEST--
Bug #73448 odbc_errormsg returns trash, always 513 bytes
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

$sqlCommandList = array(
	"/* empty batch is without error */",
	"/* non existent procedure xy */ execute xy",
	"/* empty batch,error message is not empty */",
	"/* valid select with result */ select * from sys.sysobjects",
	"/* another erroneous query */ SELECT * FROM zwiebelfleisch",
	"/* valid select with result */ select * from sys.sysobjects",
);

foreach ($sqlCommandList as $exampleNumber => $sql) {
	$r = @odbc_exec($conn, $sql);

	if (false === $r) {
		$e = odbc_errormsg($conn);
		$n = odbc_error($conn);

		var_dump($sql, $n, $e, strlen($e));	
		echo "\n";
	}

	if ($r) {
		odbc_free_result($r);
	}

}

odbc_close($conn);
?>
==DONE==
--EXPECT--
string(42) "/* non existent procedure xy */ execute xy"
string(5) "37000"
string(84) "[Microsoft][ODBC SQL Server Driver][SQL Server]Could not find stored procedure 'xy'."
int(84)

string(58) "/* another erroneous query */ SELECT * FROM zwiebelfleisch"
string(5) "S0002"
string(84) "[Microsoft][ODBC SQL Server Driver][SQL Server]Invalid object name 'zwiebelfleisch'."
int(84)

==DONE==

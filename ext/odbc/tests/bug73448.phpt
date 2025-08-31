--TEST--
Bug #73448 odbc_errormsg returns trash, always 513 bytes
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--CONFLICTS--
odbc
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

        var_dump($sql, $n, $e);
        echo "\n";
    }

    if ($r) {
        odbc_free_result($r);
    }

}

odbc_close($conn);
?>
--EXPECTF--
string(42) "/* non existent procedure xy */ execute xy"
string(5) "37000"
string(%d) "[Microsoft][%s][SQL Server]Could not find stored procedure 'xy'."

string(58) "/* another erroneous query */ SELECT * FROM zwiebelfleisch"
string(5) "S0002"
string(%d) "[Microsoft][%s][SQL Server]Invalid object name 'zwiebelfleisch'."


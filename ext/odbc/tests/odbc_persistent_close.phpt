--TEST--
odbc_pconnect(): Make sure closing a persistent connection works
--EXTENSIONS--
odbc
--SKIPIF--
<?php

include 'skipif.inc';

// The test can affect multiple drivers, but testing it is driver-specific.
// Since CI runs ODBC tests with SQL Server, just use an SQL Server specific way of testing.
$conn = odbc_connect($dsn, $user, $pass);
$result = @odbc_exec($conn, "SELECT @@Version");
if ($result) {
    $array = odbc_fetch_array($result);
    $info = (string) reset($array);
    if (!str_contains($info, "Microsoft SQL Server")) {
       echo "skip MS SQL specific test";
    }
}
?>
--FILE--
<?php

include 'config.inc';

// set a session specific variable via CONTEXT_INFO, if we get the same connection again, it should be identical
function set_context_info($conn, $string) {
	$hexstring = bin2hex($string);
	return odbc_exec($conn, "SET CONTEXT_INFO 0x$hexstring");
}

function fetch_context_info($conn) {
	$stmt = odbc_exec($conn, "SELECT CONTEXT_INFO() AS CONTEXT_INFO");
	if (!$stmt) {
		return false;
	}
	$res = odbc_fetch_array($stmt);
	if ($res) {
		// this is a binary, so we get a bunch of nulls at the end
		return $res["CONTEXT_INFO"] ? trim($res["CONTEXT_INFO"]) : null;
	} else {
		return false;
	}
}

// run 1: set expectations
$conn = odbc_pconnect($dsn, $user, $pass);
set_context_info($conn, "PHP odbc_pconnect test");
var_dump(fetch_context_info($conn));

// run 2: reuse same connection (imagine this is another request)
$conn = odbc_pconnect($dsn, $user, $pass);
var_dump(fetch_context_info($conn));

// run 3: close it and see if it's the same connection
odbc_close($conn);
$conn = odbc_pconnect($dsn, $user, $pass);
var_dump(fetch_context_info($conn));

?>
--EXPECT--
string(22) "PHP odbc_pconnect test"
string(22) "PHP odbc_pconnect test"
NULL

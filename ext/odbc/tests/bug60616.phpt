--TEST--
odbc_exec(): Getting accurate unicode data from query
--SKIPIF--
<?php include 'skipif.inc'; ?>
<?php 
	if ("unixODBC" != ODBC_TYPE) {
		die("skip ODBC_TYPE != unixODBC");
	}
?>
--FILE--
<?php

// Test strings
mb_internal_encoding("EUC_JP");
$euc_jp_base64 = 'pdal6aWkpcCl676uyqo=';
$euc_jp = base64_decode($euc_jp_base64);
$ascii = 'abcdefghijklmnopqrstuvwxyz;]=#0123456789';

include 'config.inc';
ini_set("odbc.defaultlrl", 4); // Set artifically low

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE DATABASE odbcTEST ENCODING=\'EUC_JP\'');

odbc_exec($conn, 'CREATE TABLE FOO (ID INT, CHAR_COL CHAR(200), VARCHAR_COL VARCHAR(200), TEXT_COL TEXT)');

odbc_exec($conn, "INSERT INTO FOO(ID, CHAR_COL, VARCHAR_COL, TEXT_COL) VALUES (1, '$euc_jp', '$euc_jp', '$euc_jp')");
odbc_exec($conn, "INSERT INTO FOO(ID, CHAR_COL, VARCHAR_COL, TEXT_COL) VALUES (2, '$ascii', '$ascii', '$ascii')");

$res = odbc_exec($conn, 'SELECT * FROM FOO ORDER BY ID ASC');

while(odbc_fetch_row($res)) {
	$char_col = odbc_result($res, "CHAR_COL");
	$varchar_col = odbc_result($res, "VARCHAR_COL");
	$id = odbc_result($res, "ID");
	$text_col = "";
	while (($chunk=odbc_result($res, "TEXT_COL")) !== false) {
		$text_col .= $chunk;
	}

	if ($id == 1) {
		$euc_jp_check = $euc_jp . str_repeat(" ", (200 - mb_strlen($euc_jp)));
		if (strcmp($char_col, $euc_jp_check) == 0 && strcmp($varchar_col, $euc_jp) == 0 &&
			strcmp($text_col, $euc_jp) == 0) {
			print "EUC-JP matched\n";
		} else {
			print "EUC-JP mismatched\n";
		}
	} else {
		$ascii_check = $ascii . str_repeat(" ", (200 - strlen($ascii)));
		if (strcmp($char_col, $ascii_check) == 0 && strcmp($varchar_col, $ascii) == 0 &&
			strcmp($text_col, $ascii) == 0) {
			print "ASCII matched\n";
		} else {
			print "ASCII mismatched\n";
		}
	}
}

?>
--EXPECT--
EUC-JP matched
ASCII matched
--CLEAN--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'DROP TABLE FOO');
odbc_exec($conn, 'DROP DATABASE odbcTEST');

?>

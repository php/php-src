--TEST--
dbx_connect
--SKIPIF--
<?php 
include_once("skipif.inc");
?>
--FILE--
<?php 
include_once("dbx_test.p");
$nonexisting_database="nonexisting_database";
$nonexisting_username="nonexisting_username";
$nonexisting_password="nonexisting_password";
$dlo = dbx_connect($module_name, $host, $database, $username, $password);
if ($dlo!=0) {
	print('connect using string ok'."\n");
	dbx_close($dlo);
	}
$dlo = dbx_connect($module, $host, $database, $username, $password);
if ($dlo!=0) {
	print('connect using constant ok'."\n");
	dbx_close($dlo);
	}
$dlo = @dbx_connect($module, $host, $nonexisting_database, $username, $password);
if ($dlo==0) {
	print('connect to non-existing database failed, so it\'s ok'."\n");
	}
else {
    print_r($dlo);
	dbx_close($dlo);
    }
$dlo = @dbx_connect($module, $host, $database, $nonexisting_username, $nonexisting_password);
if ($dlo==0) {
	print('connect with false username/password combi failed, so it\'s ok'."\n");
	}
else {
    print_r($dlo);
	dbx_close($dlo);
    }
$dlo = dbx_connect($module_name, $host, $database, $username, $password, DBX_PERSISTENT);
if ($dlo!=0) {
	print('persistent connect using string ok'."\n");
	dbx_close($dlo);
	}
$dlo = dbx_connect($module, $host, $database, $username, $password, DBX_PERSISTENT);
if ($dlo!=0) {
	print('persistent connect using constant ok'."\n");
	dbx_close($dlo);
	}
$dlo = @dbx_connect($module, $host, $nonexisting_database, $username, $password, DBX_PERSISTENT);
if ($dlo==0) {
	print('persistent connect to non-existing database failed, so it\'s ok'."\n");
	}
else {
    print_r($dlo);
	dbx_close($dlo);
    }
$dlo = @dbx_connect($module, $host, $database, $nonexisting_username, $nonexisting_password, DBX_PERSISTENT);
if ($dlo==0) {
	print('persistent connect with false username/password combi failed, so it\'s ok'."\n");
	}
else {
    print_r($dlo);
	dbx_close($dlo);
    }
$dlo = @dbx_connect($module, $host, $database, $username, $password, DBX_PERSISTENT, "12many");
if ($dlo==0) {
	print('too many parameters: connect failure works ok'."\n");
	}
else {
    print_r($dlo);
	dbx_close($dlo);
    }
$dlo = @dbx_connect($module, $host, $database, $username);
if ($dlo==0) {
	print('too few parameters: connect failure works ok'."\n");
	}
else {
    print_r($dlo);
	dbx_close($dlo);
    }
$dlo1 = dbx_connect($module, $host, $database, $username, $password);
$dlo2 = dbx_connect($module, $host, $database, $username, $password);
if ($dlo1!=0 && $dlo2!=0) {
	print('multiple connects ok'."\n");
	dbx_close($dlo1);
	dbx_close($dlo2);
	}
$dlo1 = dbx_connect($module, $host, $database, $username, $password);
$dlo2 = @dbx_connect($module, $host, $nonexisting_database, $username, $password);
if ($dlo1!=0 && $dlo2==0) {
    print('multiple connects (2nd fails on database-name) ok'."\n");
	dbx_close($dlo1);
	}
?>
--EXPECT--
connect using string ok
connect using constant ok
connect to non-existing database failed, so it's ok
connect with false username/password combi failed, so it's ok
persistent connect using string ok
persistent connect using constant ok
persistent connect to non-existing database failed, so it's ok
persistent connect with false username/password combi failed, so it's ok
too many parameters: connect failure works ok
too few parameters: connect failure works ok
multiple connects ok
multiple connects (2nd fails on database-name) ok
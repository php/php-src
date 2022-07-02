--TEST--
Bug ##76452 (Crash while parsing blob data in firebird_fetch_blob)
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
require_once "payload_server.inc";

$address = run_server(__DIR__ . "/bug_76452.data");

// no need to change the credentials; we're running against a falke server
$dsn = "firebird:dbname=inet://$address/test";
$username = 'SYSDBA';
$password = 'masterkey';

$dbh = new PDO($dsn, $username, $password, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
$query = $dbh->prepare("select * from test");
$query->execute();
var_dump($query->fetch());
?>
--EXPECT--
array(4) {
  ["AAA"]=>
  string(4) "hihi"
  [0]=>
  string(4) "hihi"
  ["BBBB"]=>
  NULL
  [1]=>
  NULL
}

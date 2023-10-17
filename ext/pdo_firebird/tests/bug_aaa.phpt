--TEST--
PDO_Firebird: cursor should not be marked as opened on singleton statements
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php
require 'testdb.inc';

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
@$dbh->exec('drop table test_aaa');
$dbh->exec('create table test_aaa (id integer)');
$S = $dbh->prepare('insert into test_aaa (id) values (:id) returning id');
$S->execute(['id' => 1]);
$S->execute(['id' => 2]);
unset($S);
unset($dbh);
echo 'OK';
?>
--CLEAN--
<?php
require 'testdb.inc';
$dbh->exec("DROP TABLE test_aaa");
?>
--EXPECT--
OK

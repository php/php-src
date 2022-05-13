--TEST--
mysqli_stmt_execute() - bind in execute - not supported with libmysql
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
if ($IS_MYSQLND) {
    die("skip only applicable for libmysqlclient");
}
?>
--FILE--
<?php
require_once "connect.inc";

require 'table.inc';

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

// first, control case
$id = 1;
$abc = 'abc';
$stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
$stmt->bind_param('sss', ...[&$abc, 42, $id]);
$stmt->execute();
$stmt->bind_result($v1, $v2, $v3);
$stmt->fetch();
assert(['label'=>$v1, 'anon'=>$v2, 'num'=>$v3] === ['label'=>'a', 'anon'=>'abc', 'num'=>'42']);
$stmt = null;

// 1. same as the control case, but skipping the middle-man (bind_param)
$stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
try {
    $stmt->execute([&$abc, 42, $id]);
} catch (ArgumentCountError $e) {
    echo '[001] '.$e->getMessage()."\n";
}
$stmt = null;

mysqli_close($link);
?>
--CLEAN--
<?php
require_once "clean_table.inc";
?>
--EXPECT--
[001] Binding parameters in execute is not supported with libmysqlclient

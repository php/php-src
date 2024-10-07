--TEST--
Bug #15604 It is not possible to pass a NULL value as an input parameter if the field is marked as NOT NULL.
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
require_once 'testdb.inc';

$dbh = getDbConnection();

$dbh->exec('
recreate table t_bug_15604 (
  id bigint not null,
  a int not null,
  b int,
  constraint pk_bug_15604 primary key(id)
)
');

$dbh->exec('recreate sequence g_bug_15604');

$dbh->exec(<<<'SQL'
create or alter trigger t_bug_15604_bi0 for t_bug_15604
active before insert position 0
as
begin
  if (new.id is null) then
    new.id = next value for g_bug_15604;
end
SQL
);

$stmt = $dbh->prepare('insert into t_bug_15604(id, a, b) values(?, ?, ?)');
$stmt->execute([null, 1, 2]);
$stmt->execute([2, 2, null]);
unset($stmt);

$stmt2 = $dbh->prepare('SELECT id, a, b FROM t_bug_15604 WHERE id = ?');

$stmt2->execute([null]); 
$data = $stmt2->fetch(\PDO::FETCH_ASSOC);
$stmt2->closeCursor();
var_dump($data);

$stmt2->execute([2]); 
$data = $stmt2->fetch(\PDO::FETCH_ASSOC);
$stmt2->closeCursor();
var_dump($data);

unset($stmt2);

echo "\nOK\n";
?>
--CLEAN--
<?php
require_once 'testdb.inc';
$dbh = getDbConnection();
@$dbh->exec('drop table t_bug_15604');
@$dbh->exec('drop sequence g_bug_15604');
unset($dbh);
?>
--EXPECTF--
bool(false)
array(3) {
  ["ID"]=>
  %r(int\(2\)|string\(1\) "2")%r
  ["A"]=>
  int(2)
  ["B"]=>
  NULL
}

OK

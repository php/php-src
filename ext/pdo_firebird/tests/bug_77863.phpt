--TEST--
PDO_Firebird: Bug #76488 PDO Firebird does not support boolean datatype in input parameters
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php

require 'testdb.inc';

$sql = <<<SQL
with t(b, s) as (
  select true, 'true' from rdb\$database
  union all
  select false, 'false' from rdb\$database
  union all
  select unknown, 'unknown' from rdb\$database
)
select trim(s) as s from t where b is not distinct from :p
SQL;

try {
  $dbh = getDbConnection();
  $query = $dbh->prepare($sql);

  // PDO::PARAM_BOOL
  $query->bindValue('p', 0, PDO::PARAM_BOOL);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', 1, PDO::PARAM_BOOL);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', false, PDO::PARAM_BOOL);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', true, PDO::PARAM_BOOL);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', 'false', PDO::PARAM_BOOL);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', 'True', PDO::PARAM_BOOL);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', null, PDO::PARAM_BOOL);
  $query->execute();
  var_dump($query->fetchColumn(0));

  // PDO::PARAM_STR
  $query->bindValue('p', false, PDO::PARAM_STR);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', true, PDO::PARAM_STR);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', 0, PDO::PARAM_STR);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', 1, PDO::PARAM_STR);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', 'false', PDO::PARAM_STR);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', 'true', PDO::PARAM_STR);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', null, PDO::PARAM_STR);
  $query->execute();
  var_dump($query->fetchColumn(0));

  // PDO::PARAM_INT
  $query->bindValue('p', false, PDO::PARAM_INT);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', true, PDO::PARAM_INT);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', 0, PDO::PARAM_INT);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', 1, PDO::PARAM_INT);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', 'false', PDO::PARAM_INT);
  $query->execute();
  var_dump($query->fetchColumn(0));

  $query->bindValue('p', 'true', PDO::PARAM_INT);
  $query->execute();
  var_dump($query->fetchColumn(0));

  echo "OK\n";
}
catch(Exception $e) {
    echo $e->getMessage() . '<br>';
    echo $e->getTraceAsString();
}
?>
--EXPECT--
string(5) "false"
string(4) "true"
string(5) "false"
string(4) "true"
string(5) "false"
string(4) "true"
string(7) "unknown"
string(5) "false"
string(4) "true"
string(5) "false"
string(4) "true"
string(5) "false"
string(4) "true"
string(7) "unknown"
string(5) "false"
string(4) "true"
string(5) "false"
string(4) "true"
string(5) "false"
string(4) "true"
OK

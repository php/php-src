--TEST--
PDO_Firebird: Bug #76488 Memory leak when fetching a BLOB field
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

$sql = '
with recursive r(n) as (
  select 1 from rdb$database
  union all
  select n+1 from r where n < 1000
)
select n,
       cast(lpad(\'A\', 8000, \'A\') as BLOB sub_type TEXT) as SRC
from r
';

$dbh = getDbConnection();

for ($i = 0; $i < 10; $i++) {
    $sth = $dbh->prepare($sql);
    $sth->execute();
    $rows = $sth->fetchAll();
    unset($rows);
    unset($sth);
}
unset($dbh);
echo "OK";
?>
--EXPECT--
OK

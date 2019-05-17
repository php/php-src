--TEST--
PDO_Firebird: Bug #76488 Memory leak when fetching a BLOB field
--SKIPIF--
<?php require('skipif.inc'); ?>
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

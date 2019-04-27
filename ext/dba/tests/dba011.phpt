--TEST--
DBA argument tests
--SKIPIF--
<?php
require_once(__DIR__ .'/skipif.inc');
die("info $HND handler used");
?>
--FILE--
<?php
require_once(__DIR__ .'/test.inc');
echo "database handler: $handler\n";
var_dump(dba_open($db_file, 'n'));
var_dump(dba_open($db_file, 'n', 'bogus'));
var_dump(dba_open($db_file, 'q', $handler));
var_dump(dba_open($db_file, 'nq', $handler));
var_dump(dba_open($db_file, 'n', $handler, 2, 3, 4, 5, 6, 7, 8));
?>
--CLEAN--
<?php
require(__DIR__ .'/clean.inc');
?>
--EXPECTF--
database handler: flatfile
resource(%d) of type (dba)

Warning: dba_open(%stest0.dbm,n): No such handler: bogus in %sdba011.php on line %d
bool(false)

Warning: dba_open(%stest0.dbm,q): Illegal DBA mode in %sdba011.php on line %d
bool(false)

Warning: dba_open(%stest0.dbm,nq): Illegal DBA mode in %sdba011.php on line %d
bool(false)
resource(%d) of type (dba)

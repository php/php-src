--TEST--
PDO_DBLIB: Does not force correct dateformat
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require __DIR__ . '/config.inc';
?>
--FILE--
<?php
require __DIR__ . '/config.inc';
$db->query('set dateformat ymd');
$rs = $db->query("select cast('1950-01-18 23:00:00' as smalldatetime) as sdt, cast('2030-01-01 23:59:59' as datetime) as dt");
var_dump($rs->fetchAll(PDO::FETCH_ASSOC));
echo "Done.\n";
?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    ["sdt"]=>
    string(19) "1950-01-18 23:00:00"
    ["dt"]=>
    string(19) "2030-01-01 23:59:59"
  }
}
Done.

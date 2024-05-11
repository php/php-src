--TEST--
PDO PgSQL float value
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

$stmt = $db->query(<<<'SQL'
select cast(0.4 as float4) as value4,
cast(0.8 as float8) as value8,
cast('NaN' as float4) as valuenan,
cast('Infinity' as float4) as valueinfinity,
cast('-Infinity' as float4) as valueninfinity
SQL);

var_dump($stmt->fetchAll());
var_dump($stmt->getColumnMeta(0));
var_dump($stmt->getColumnMeta(1));
var_dump($stmt->getColumnMeta(2));
var_dump($stmt->getColumnMeta(3));
var_dump($stmt->getColumnMeta(4));
?>
--EXPECT--
array(1) {
  [0]=>
  array(5) {
    ["value4"]=>
    float(0.4)
    ["value8"]=>
    float(0.8)
    ["valuenan"]=>
    float(NAN)
    ["valueinfinity"]=>
    float(INF)
    ["valueninfinity"]=>
    float(-INF)
  }
}
array(7) {
  ["pgsql:oid"]=>
  int(700)
  ["pgsql:table_oid"]=>
  int(0)
  ["native_type"]=>
  string(6) "float4"
  ["pdo_type"]=>
  int(2)
  ["name"]=>
  string(6) "value4"
  ["len"]=>
  int(4)
  ["precision"]=>
  int(-1)
}
array(7) {
  ["pgsql:oid"]=>
  int(701)
  ["pgsql:table_oid"]=>
  int(0)
  ["native_type"]=>
  string(6) "float8"
  ["pdo_type"]=>
  int(2)
  ["name"]=>
  string(6) "value8"
  ["len"]=>
  int(8)
  ["precision"]=>
  int(-1)
}
array(7) {
  ["pgsql:oid"]=>
  int(700)
  ["pgsql:table_oid"]=>
  int(0)
  ["native_type"]=>
  string(6) "float4"
  ["pdo_type"]=>
  int(2)
  ["name"]=>
  string(8) "valuenan"
  ["len"]=>
  int(4)
  ["precision"]=>
  int(-1)
}
array(7) {
  ["pgsql:oid"]=>
  int(700)
  ["pgsql:table_oid"]=>
  int(0)
  ["native_type"]=>
  string(6) "float4"
  ["pdo_type"]=>
  int(2)
  ["name"]=>
  string(13) "valueinfinity"
  ["len"]=>
  int(4)
  ["precision"]=>
  int(-1)
}
array(7) {
  ["pgsql:oid"]=>
  int(700)
  ["pgsql:table_oid"]=>
  int(0)
  ["native_type"]=>
  string(6) "float4"
  ["pdo_type"]=>
  int(2)
  ["name"]=>
  string(14) "valueninfinity"
  ["len"]=>
  int(4)
  ["precision"]=>
  int(-1)
}

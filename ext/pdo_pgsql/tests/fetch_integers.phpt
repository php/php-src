--TEST--
PDO PgSQL: integer columns at their boundaries
--EXTENSIONS--
pdo_pgsql
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) die('skip 64-bit only');
require __DIR__ . '/config.inc';
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

$stmt = $db->query(<<<'SQL'
SELECT 0::int2, (-1)::int2, (-32768)::int2, 32767::int2,
       (-2147483648)::int4, 2147483647::int4, 7::int4,
       999999999999999999::int8, (-999999999999999999)::int8,
       1000000000000000000::int8, (-1000000000000000000)::int8,
       9223372036854775807::int8, '-9223372036854775808'::int8
SQL);
var_dump($stmt->fetch(PDO::FETCH_NUM));
?>
--EXPECT--
array(13) {
  [0]=>
  int(0)
  [1]=>
  int(-1)
  [2]=>
  int(-32768)
  [3]=>
  int(32767)
  [4]=>
  int(-2147483648)
  [5]=>
  int(2147483647)
  [6]=>
  int(7)
  [7]=>
  int(999999999999999999)
  [8]=>
  int(-999999999999999999)
  [9]=>
  int(1000000000000000000)
  [10]=>
  int(-1000000000000000000)
  [11]=>
  int(9223372036854775807)
  [12]=>
  int(-9223372036854775808)
}

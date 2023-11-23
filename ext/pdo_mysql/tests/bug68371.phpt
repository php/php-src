--TEST--
PDO MySQL Bug #38671 (PDO#getAttribute() cannot be called with platform-specific attribute names)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$pdo = MySQLPDOTest::factory();
$pdo->setAttribute (\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);

$attrs = [
    // Extensive test: default value and set+get values
    PDO::ATTR_EMULATE_PREPARES		=> array(null, 1, 0),
    PDO::MYSQL_ATTR_DIRECT_QUERY	=> array(null, 0, 1),
    PDO::MYSQL_ATTR_USE_BUFFERED_QUERY => array(null, false, true),

    // Just test the default
    PDO::ATTR_AUTOCOMMIT			=> [null],
    PDO::ATTR_PREFETCH				=> [null],
    PDO::ATTR_TIMEOUT				=> [null],
    PDO::ATTR_ERRMODE				=> [null],
    PDO::ATTR_SERVER_VERSION		=> [null],
    PDO::ATTR_CLIENT_VERSION		=> [null],
    PDO::ATTR_SERVER_INFO			=> [null],
    PDO::ATTR_CONNECTION_STATUS		=> [null],
    PDO::ATTR_CASE					=> [null],
    PDO::ATTR_CURSOR_NAME			=> [null],
    PDO::ATTR_CURSOR				=> [null],
    PDO::ATTR_ORACLE_NULLS			=> [null],
    PDO::ATTR_PERSISTENT			=> [null],
    PDO::ATTR_STATEMENT_CLASS		=> [null],
    PDO::ATTR_FETCH_TABLE_NAMES		=> [null],
    PDO::ATTR_FETCH_CATALOG_NAMES	=> [null],
    PDO::ATTR_DRIVER_NAME			=> [null],
    PDO::ATTR_STRINGIFY_FETCHES		=> [null],
    PDO::ATTR_MAX_COLUMN_LEN		=> [null],
    PDO::ATTR_DEFAULT_FETCH_MODE	=> [null],
];

foreach ($attrs as $a => $vals) {
    foreach ($vals as $v) {
        try {
            if (!isset($v)) {
                var_dump($pdo->getAttribute($a));
            } else {
                $pdo->setAttribute($a, $v);
                if ($pdo->getAttribute($a) === $v) {
                    echo "OK\n";
                } else {
                    throw new \Exception('KO');
                }
            }
        } catch (\Exception $e) {
            if ($e->getCode() == 'IM001') {
                echo "ERR\n";
            } else {
                echo "ERR {$e->getMessage()}\n";
            }
        }
    }
}

?>
--EXPECTF--
int(1)
OK
OK
int(0)
OK
OK
bool(true)
OK
OK
int(1)
ERR
ERR
int(2)
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
int(2)
ERR
ERR
int(0)
bool(false)
array(1) {
  [0]=>
  string(12) "PDOStatement"
}
ERR
ERR
string(5) "mysql"
ERR
ERR
int(4)

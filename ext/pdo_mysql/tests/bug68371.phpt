--TEST--
PDO MySQL Bug #38671 (PDO#getAttribute() cannot be called with platform-specific attribute names)
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$pdo = MySQLPDOTest::factory();
$pdo->setAttribute (\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);

$attrs = array(
    // Extensive test: default value and set+get values
	PDO::ATTR_EMULATE_PREPARES		=> array(null, 1, 0),
	PDO::MYSQL_ATTR_DIRECT_QUERY	=> array(null, 0, 1),
	PDO::MYSQL_ATTR_USE_BUFFERED_QUERY => array(null, 0, 1),

    // Just test the default
	PDO::ATTR_AUTOCOMMIT			=> array(null),
	PDO::ATTR_PREFETCH				=> array(null),
	PDO::ATTR_TIMEOUT				=> array(null),
	PDO::ATTR_ERRMODE				=> array(null),
	PDO::ATTR_SERVER_VERSION		=> array(null),
	PDO::ATTR_CLIENT_VERSION		=> array(null),
	PDO::ATTR_SERVER_INFO			=> array(null),
	PDO::ATTR_CONNECTION_STATUS		=> array(null),
	PDO::ATTR_CASE					=> array(null),
	PDO::ATTR_CURSOR_NAME			=> array(null),
	PDO::ATTR_CURSOR				=> array(null),
	PDO::ATTR_ORACLE_NULLS			=> array(null),
	PDO::ATTR_PERSISTENT			=> array(null),
	PDO::ATTR_STATEMENT_CLASS		=> array(null),
	PDO::ATTR_FETCH_TABLE_NAMES		=> array(null),
	PDO::ATTR_FETCH_CATALOG_NAMES	=> array(null),
	PDO::ATTR_DRIVER_NAME			=> array(null),
	PDO::ATTR_STRINGIFY_FETCHES		=> array(null),
	PDO::ATTR_MAX_COLUMN_LEN		=> array(null),
	PDO::ATTR_DEFAULT_FETCH_MODE	=> array(null),
);

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
int(1)
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


--TEST--
MySQL PDOStatement->execute()/fetch(), Non-SELECT
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

    try {

        class MyPDO extends PDO {

            public function __construct() {
                $this->protocol();
                return call_user_func_array(array($this, 'parent::__construct'), func_get_args());
            }

            public function exec($statement): int|false {
                $this->protocol();
                return parent::exec($statement);
            }

            public function query(...$args): PDOStatement|false {
                $this->protocol();
                return parent::query(...$args);
            }

            public function __call($method, $args) {
                print "__call(".var_export($method,true).", ".var_export($args, true).")\n";
                // $this->protocol();
            }

            private function protocol() {
                $stack = debug_backtrace();
                if (!isset($stack[1]))
                    return;

                printf("%s(", $stack[1]['function']);
                $args = '';
                foreach ($stack[1]['args'] as $k => $v)
                    $args .= sprintf("%s, ", var_export($v, true));
                if ($args != '')
                    printf("%s", substr($args, 0, -2));
                printf(")\n");
            }

        }

        $db = new MyPDO(PDO_MYSQL_TEST_DSN, PDO_MYSQL_TEST_USER, PDO_MYSQL_TEST_PASS);
        $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
        $db->exec('DROP TABLE IF EXISTS test');
        $db->exec('CREATE TABLE test(id INT)');
        $db->exec('INSERT INTO test(id) VALUES (1), (2)');
        $stmt = $db->query('SELECT * FROM test ORDER BY id ASC');
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
        var_dump($stmt->fetch());
        $db->intercept_call();


    } catch (PDOException $e) {
        printf("[001] %s [%s] %s\n",
            $e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
    }

    $db->exec('DROP TABLE IF EXISTS test');
    print "done!\n";
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
__construct('%S', '%S', %s)

Deprecated: Callables of the form ["MyPDO", "parent::__construct"] are deprecated in %s on line %d
exec('DROP TABLE IF EXISTS test')
exec('CREATE TABLE test(id INT)')
exec('INSERT INTO test(id) VALUES (1), (2)')
query('SELECT * FROM test ORDER BY id ASC')
array(2) {
  [0]=>
  array(1) {
    ["id"]=>
    string(1) "1"
  }
  [1]=>
  array(1) {
    ["id"]=>
    string(1) "2"
  }
}
bool(false)
__call('intercept_call', array (
))
exec('DROP TABLE IF EXISTS test')
done!

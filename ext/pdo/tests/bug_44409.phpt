--TEST--
PDO Common: Bug #44409 (PDO::FETCH_SERIALIZE calls __construct())
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$db->exec("CREATE TABLE test44409 (dat varchar(100))");
$db->exec("INSERT INTO test44409 (dat) VALUES ('Data from DB')");

class bug44409 implements Serializable
{
    public function __construct()
    {
        printf("Method called: %s()\n", __METHOD__);
    }

    public function serialize()
    {
        return "any data from serizalize()";
    }

    public function unserialize($dat)
    {
        printf("Method called: %s(%s)\n", __METHOD__, var_export($dat, true));
    }
}

$stmt = $db->query("SELECT * FROM test44409");

print_r($stmt->fetchAll(PDO::FETCH_CLASS|PDO::FETCH_SERIALIZE, "bug44409"));

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test44409");
?>
--EXPECTF--
Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Deprecated: PDOStatement::fetchAll(): The PDO::FETCH_SERIALIZE mode is deprecated in %s on line %d
Method called: bug44409::unserialize('Data from DB')
Array
(
    [0] => bug44409 Object
        (
        )

)

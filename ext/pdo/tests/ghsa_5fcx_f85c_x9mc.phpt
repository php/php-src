--TEST--
PDO Common: GHSA-5fcx-f85c-x9mc (Memory leak inside PDO could lead to a DoS)
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
// see https://github.com/php/php-src/security/advisories/GHSA-5fcx-f85c-x9mc

class TestStmt extends PDOStatement
{
    public $name;
}

$db = new PDO(
    'mysql:host=db.sakiot.com;dbname=db;',
    'saki',
    'password',
    [PDO::ATTR_STATEMENT_CLASS => [TestStmt::class]],
);

$db->exec('CREATE TABLE ghsa_5fcx_f85c_x9mc (name varchar(255))');
$db->exec('INSERT INTO ghsa_5fcx_f85c_x9mc (name) VALUES ("test_name")');

$stmt = $db->query('SELECT name FROM ghsa_5fcx_f85c_x9mc');
$t = $stmt;
$stmt->setFetchMode(PDO::FETCH_INTO, $stmt);
$stmt->fetch();
echo "done!\n";
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, 'ghsa_5fcx_f85c_x9mc');
?>
--EXPECT--
done!

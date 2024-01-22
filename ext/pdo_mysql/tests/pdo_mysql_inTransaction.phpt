--TEST--
MySQL PDO class inTransaction
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

const BEGIN = ['BEGIN', 'START TRANSACTION'];
const END = ['COMMIT', 'ROLLBACK'];

$db = MySQLPDOTest::factory();
// $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false); // mysql does not support
foreach (BEGIN as $begin) {
    foreach (END as $end) {
        foreach (['exec', 'query', 'execute'] as $w) {
            foreach ([$begin, $end] as $command) {
                switch ($w) {
                    case 'exec':
                        $db->exec($command);
                        break;
                    case'query':
                        $db->query($command)->execute();
                        break;
                    case 'execute':
                        /* EMULATE_PREPARES = QUERY */
                        $db->prepare($command)->execute();
                        break;
                    default:
                        assert(0);
                }
                var_dump($db->inTransaction());
            }
        }
    }
}
echo "\n";

// Mixing PDO transaction API and explicit queries.
foreach (END as $end) {
    $db->beginTransaction();
    var_dump($db->inTransaction());
    $db->exec($end);
    var_dump($db->inTransaction());
}

$db->exec('START TRANSACTION');
var_dump($db->inTransaction());
$db->rollBack();
var_dump($db->inTransaction());
$db->exec('START TRANSACTION');
var_dump($db->inTransaction());
$db->commit();
var_dump($db->inTransaction());
echo "\n";

// DDL query causes an implicit commit.
$db->beginTransaction();
var_dump($db->inTransaction());
$db->exec('DROP TABLE IF EXISTS pdo_mysql_inTransaction');
var_dump($db->inTransaction());

// We should be able to start a new transaction after the implicit commit.
$db->beginTransaction();
var_dump($db->inTransaction());
$db->commit();
var_dump($db->inTransaction());

?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)

bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)

bool(true)
bool(false)
bool(true)
bool(false)

--TEST--
MySQL PDO class inTransaction
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

const BEGIN = ['BEGIN', 'START TRANSACTION'];
const END = ['COMMIT', 'ROLLBACK'];

$db = MySQLPDOTest::factory();
// $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false); // mysql does not support
for ($b = 0; $b < count(BEGIN); $b++) {
    for ($e = 0; $e < count(END); $e++) {
        foreach (['exec', 'query', 'execute'] as $w) {
            foreach ([BEGIN[$b], END[$e]] as $command) {
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

--TEST--
MySQL PDO class inTransaction
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';

const BEGIN = ['BEGIN', 'START TRANSACTION'];
const END = ['COMMIT', 'ROLLBACK'];

$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
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

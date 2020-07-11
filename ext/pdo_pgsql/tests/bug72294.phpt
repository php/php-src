--TEST--
Bug #72294 Segmentation fault/invalid pointer in connection with pgsql_stmt_dtor
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';

function handleError($errno, $errstr, $errfile, $errline)
{
    if (!($errno & error_reporting())) {
        return false;
    }

    throw new RuntimeException( $errstr, $errno );
}

abstract class PHPUnit_Framework_TestCase
{
    private $name = null;
    private $result;

    public function run(PHPUnit_Framework_TestResult $result = null)
    {
        $result->run($this);
    }

    public function runBare()
    {
        $class  = new ReflectionClass($this);
        $method = $class->getMethod($this->name);
        $method->invoke($this);

        if( $x ) {
        }
    }

    public function setName($name)
    {
        $this->name = $name;
    }
}

class PHPUnit_Framework_TestFailure
{
    private $testName;

    protected $failedTest;

    protected $thrownException;

    public function __construct( $failedTest, $t)
    {
        if ($failedTest instanceof PHPUnit_Framework_SelfDescribing) {
            $this->testName = $failedTest->toString();
        } else {
            $this->testName = get_class($failedTest);
        }

        $this->thrownException = $t;
    }
}

class PHPUnit_Framework_TestResult
{
    public function run( $test)
    {
        $error      = false;

        $oldErrorHandler = set_error_handler(
            'handleError',
            E_ALL
        );

        try {
            $test->runBare();
        } catch (RuntimeException $e) {
            $error = true;
        }

        restore_error_handler();

        if ($error === true) {
            $this->errors[] = new PHPUnit_Framework_TestFailure($test, $e);
        }
    }
}

$result = new PHPUnit_Framework_TestResult();

class PreparedStatementCache
{
    private $cached_statements = array();

    public function prepare( $pdo, $sql )
    {
        //return $pdo->prepare( $sql );
        $this->cached_statements[$sql] = $pdo->prepare( $sql );

    return $this->cached_statements[$sql];
    }
}

class DatabaseTest extends PHPUnit_Framework_TestCase
{
    public function testIt()
    {
    $pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');

    $prepared_statement_cache = new PreparedStatementCache( $pdo );

    for( $i = 1; $i <= 300; ++$i ) {
        $statement = $prepared_statement_cache->prepare( $pdo,  <<<SQL
                SELECT $i;
SQL
        );
            $statement->execute();
    }
    }

    public function test_construct()
    {
    $pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');

    $pdo->exec( 'CREATE TEMPORARY TABLE temp_table ( test_column INT NOT NULL );' );

    $this->cache = new PreparedStatementCache( $pdo );

    $statement = $this->cache->prepare( $pdo, 'SELECT * FROM temp_table WHERE test_column > 0' );
    $statement->execute();
    }
}

$test = new DatabaseTest();
$test->setName( 'testIt' );
$test->run( $result );
$test->setName( 'test_construct' );
$test->run( $result );

?>
==NOCRASH==
--EXPECT--
==NOCRASH==

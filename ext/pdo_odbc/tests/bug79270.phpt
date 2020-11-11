--TEST--
Bug #79270 (segfault in libodbc.so.2.0.0)
--SKIPIF--
<?php
if (!extension_loaded('pdo_odbc')) die('skip pdo_odbc extension not available');
require 'ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--INI--
zend.enable_gc=0
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';

class Core {
    public $conn;
    public function __construct() {
      $this->circularReference = $this;
    }

    public function connect() {
      $this->conn = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
    }

    public $stm;

    function createStm( string $foo ) {
      $this->connect();
      $this->stm = $this->conn->prepare("SELECT 1");
    }
}

function someReflection( $function ) {
    $rParameters = (new \ReflectionFunction($function))->getParameters();
}

someReflection("implode");

$app = new Core;
$app->createStm("bar");
echo "done\n";
?>
--EXPECT--
done

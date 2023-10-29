--TEST--
PDO::ATTR_STATEMENT_CLASS
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
    $db = MySQLPDOTest::factory();
    $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
    MySQLPDOTest::createTestTable($db);

    $default =  $db->getAttribute(PDO::ATTR_STATEMENT_CLASS);
    var_dump($default);

    try {
        $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, 'foo');
    } catch (\TypeError $e) {
        echo $e->getMessage(), \PHP_EOL;
    }
    try {
        $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['classname']);
    } catch (\TypeError $e) {
        echo $e->getMessage(), \PHP_EOL;
    }
    // unknown class
    try {
        $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['classname', []]);
    } catch (\TypeError $e) {
        echo $e->getMessage(), \PHP_EOL;
    }

    // class not derived from PDOStatement
    class myclass {
        function __construct() {
            printf("myclass\n");
        }
    }

    try {
        $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['myclass', []]);
    } catch (\TypeError $e) {
        echo $e->getMessage(), \PHP_EOL;
    }

    // public constructor not allowed
    class mystatement extends PDOStatement {
        public function __construct() {
            printf("mystatement\n");
        }
    }

    try {
        if (false !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['mystatement', []])))
            printf("[006] Expecting boolean/false got %s\n", var_export($tmp, true));
    } catch (\Error $e) {
        echo get_class($e), ': ', $e->getMessage(), \PHP_EOL;
    }


    // ... but a public destructor is allowed
    class mystatement2 extends PDOStatement {
        public function __destruct() {
            printf("mystatement\n");
        }
    }

    if (true !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement2', array()))))
        printf("[007] Expecting boolean/true got %s\n", var_export($tmp, true));

    // private constructor
    class mystatement3 extends PDOStatement {
        private function __construct($msg) {
            printf("mystatement3\n");
            var_dump($msg);
        }
    }
    if (true !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement3', array('param1')))))
        printf("[008] Expecting boolean/true got %s\n", var_export($tmp, true));

    // private constructor
    class mystatement4 extends PDOStatement {
        private function __construct($msg) {
            printf("%s\n", get_class($this));
            var_dump($msg);
        }
    }
    if (true !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement4', array('param1')))))
        printf("[008] Expecting boolean/true got %s\n", var_export($tmp, true));

    var_dump($db->getAttribute(PDO::ATTR_STATEMENT_CLASS));
    $stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC LIMIT 2');

    class mystatement5 extends mystatement4 {
        public function fetchAll($fetch_style = 1, ...$fetch_args): array {
            return [];
        }
    }

    if (true !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement5', array('mystatement5')))))
        printf("[009] Expecting boolean/true got %s\n", var_export($tmp, true));
    $stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC LIMIT 2');
    var_dump($stmt->fetchAll());

    if (true !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('PDOStatement'))))
        printf("[010] Expecting boolean/true got %s\n", var_export($tmp, true));

    $stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC LIMIT 1');
    var_dump($stmt->fetchAll());

    // Yes, this is a fatal error and I want it to fail.
    abstract class mystatement6 extends mystatement5 {
    }
    try {
        $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['mystatement6', ['mystatement6']]);
        $stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC LIMIT 1');
    } catch (\Error $e) {
        echo get_class($e), ': ', $e->getMessage(), \PHP_EOL;
    }

?>
--CLEAN--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
array(1) {
  [0]=>
  string(12) "PDOStatement"
}
PDO::ATTR_STATEMENT_CLASS value must be of type array, string given
PDO::ATTR_STATEMENT_CLASS class must be a valid class
PDO::ATTR_STATEMENT_CLASS class must be a valid class
PDO::ATTR_STATEMENT_CLASS class must be derived from PDOStatement
TypeError: User-supplied statement class cannot have a public constructor
array(2) {
  [0]=>
  string(12) "mystatement4"
  [1]=>
  array(1) {
    [0]=>
    string(6) "param1"
  }
}
mystatement4
string(6) "param1"
mystatement5
string(12) "mystatement5"
array(0) {
}
array(1) {
  [0]=>
  array(4) {
    ["id"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
    ["label"]=>
    string(1) "a"
    [1]=>
    string(1) "a"
  }
}
Error: Cannot instantiate abstract class mystatement6

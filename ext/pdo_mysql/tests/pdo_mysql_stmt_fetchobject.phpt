--TEST--
MySQL PDO: PDOStatement->fetchObject()
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();

try {
    $query = "SELECT '', NULL, \"\" FROM DUAL";
    $stmt = $db->prepare($query);
    $ok = @$stmt->execute();
} catch (PDOException $e) {
    die("skip: Test cannot be run with SQL mode ANSI");
}
if (!$ok)
    die("skip: Test cannot be run with SQL mode ANSI");
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

$table = 'pdo_mysql_stmt_fetchobject';
MySQLPDOTest::createTestTable($table, $db);

try {

    $query = "SELECT id, '', NULL, \"\" FROM {$table} ORDER BY id ASC LIMIT 3";
    $stmt = $db->prepare($query);

    #[AllowDynamicProperties]
    class myclass {

        private $set_calls = 0;
        protected static $static_set_calls = 0;

        // NOTE: PDO does not care about protected
        protected $grp;

        // NOTE: PDO does not care about private and calls __construct() after __set()
        private function __construct($param1, $param2) {
            printf("myclass::__construct(%s, %s): %d / %d\n",
                $param1, $param2,
                self::$static_set_calls, $this->set_calls);
        }

        // NOTE: PDO will call __set() prior to calling __construct()
        public function __set($prop, $value) {
            $this->not_a_magic_one();
            printf("myclass::__set(%s, -%s-) %d\n",
                $prop, var_export($value, true), $this->set_calls, self::$static_set_calls);
            if ("" != $prop)
                $this->{$prop} = $value;
        }

        // NOTE: PDO can call regular methods prior to calling __construct()
        public function not_a_magic_one() {
            $this->set_calls++;
            self::$static_set_calls++;
        }

    }
    $stmt->execute();
    $rowno = 0;
    $rows[] = array();
    while (is_object($rows[] = $stmt->fetchObject('myclass', array($rowno++, $rowno))))
        ;

    var_dump($rows[$rowno - 1]);

    try {
        $stmt->fetchObject('class_does_not_exist');
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
} catch (PDOException $e) {
    // we should never get here, we use warnings, but never trust a system...
    printf("[001] %s, [%s} %s\n",
        $e->getMessage(), $db->errorInfo(), implode(' ', $db->errorInfo()));
}

print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS pdo_mysql_stmt_fetchobject');
?>
--EXPECTF--
myclass::__set(id, -'1'-) 1
myclass::__set(, -''-) 2
myclass::__set(null, -NULL-) 3
myclass::__set(, -''-) 4
myclass::__construct(0, 1): 4 / 4
myclass::__set(id, -'2'-) 1
myclass::__set(, -''-) 2
myclass::__set(null, -NULL-) 3
myclass::__set(, -''-) 4
myclass::__construct(1, 2): 8 / 4
myclass::__set(id, -'3'-) 1
myclass::__set(, -''-) 2
myclass::__set(null, -NULL-) 3
myclass::__set(, -''-) 4
myclass::__construct(2, 3): 12 / 4
object(myclass)#%d (4) {
  ["set_calls":"myclass":private]=>
  int(4)
  ["grp":protected]=>
  NULL
  ["id"]=>
  string(1) "3"
  ["null"]=>
  NULL
}
PDOStatement::fetchObject(): Argument #1 ($class) must be a valid class name, class_does_not_exist given
done!

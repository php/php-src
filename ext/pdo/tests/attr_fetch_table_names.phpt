--TEST--
PDO Common: PDO::ATTR_FETCH_TABLE_NAMES
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
if (!strncasecmp(getenv('PDOTEST_DSN'), 'dblib', strlen('dblib'))) die('skip SQL Server does not return this information normally');
// SQL Server and Db2i drivers don't seem to return this info
if (!strncasecmp(getenv('PDOTEST_DSN'), 'odbc', strlen('odbc'))) die('skip SQL Server does not return this information normally');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
    require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
    $db = PDOTest::factory();

    $table = 'pdo_attr_fetch_table_names';
    $db->exec("CREATE TABLE {$table} (id INT, label CHAR(1), PRIMARY KEY(id))");
    $db->exec("INSERT INTO {$table} (id, label) VALUES (1, 'a')");

    // Firebird returns tables in upcase, force lowering
    $db->setAttribute(PDO::ATTR_CASE, PDO::CASE_LOWER);

    $db->setAttribute(PDO::ATTR_FETCH_TABLE_NAMES, true);
    var_dump($db->getAttribute(PDO::ATTR_FETCH_TABLE_NAMES));
    $stmt = $db->query("SELECT label FROM {$table}");
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC)[0]);
    $stmt->closeCursor();

    $db->setAttribute(PDO::ATTR_FETCH_TABLE_NAMES, false);
    var_dump($db->getAttribute(PDO::ATTR_FETCH_TABLE_NAMES));
    $stmt = $db->query("SELECT label FROM {$table}");
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC)[0]);
    $stmt->closeCursor();

    print "done!";
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_attr_fetch_table_names");
?>
--EXPECT--
bool(true)
array(1) {
  ["pdo_attr_fetch_table_names.label"]=>
  string(1) "a"
}
bool(false)
array(1) {
  ["label"]=>
  string(1) "a"
}
done!

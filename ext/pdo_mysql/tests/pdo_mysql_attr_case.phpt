--TEST--
PDO::ATTR_CASE
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
    $db = MySQLPDOTest::factory();
    $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

    $table = 'pdo_mysql_attr_case';
    MySQLPDOTest::createTestTable($table, $db);

    $default =  $db->getAttribute(PDO::ATTR_CASE);
    $known = [
        PDO::CASE_LOWER => 'PDO::CASE_LOWER',
        PDO::CASE_UPPER => 'PDO::CASE_UPPER',
        PDO::CASE_NATURAL => 'PDO::CASE_NATURAL'
    ];
    if (!isset($known[$default]))
        printf("[001] getAttribute(PDO::ATTR_CASE) returns unknown value '%s'\n",
            var_export($default, true));
    else
        var_dump($known[$default]);

    // lets see what the default is...
    if (!is_object($stmt = $db->query("SELECT id, id AS 'ID_UPPER', label FROM {$table} ORDER BY id ASC LIMIT 2")))
        printf("[002] %s - %s\n",
            var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

    var_dump($stmt->fetchAll(PDO::FETCH_BOTH));

    if (true !== $db->setAttribute(PDO::ATTR_CASE, PDO::CASE_LOWER))
        printf("[003] Cannot set PDO::ATTR_CASE = PDO::CASE_LOWER, %s - %s\n",
            var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

    if (($tmp = $db->getAttribute(PDO::ATTR_CASE)) !== PDO::CASE_LOWER)
        printf("[004] getAttribute(PDO::ATTR_CASE) returns wrong value '%s'\n",
            var_export($tmp, true));

    if (true === $db->exec("ALTER TABLE {$table} ADD MiXeD CHAR(1)"))
        printf("[005] Cannot add column %s - %s\n",
            var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

    if (false === $db->exec("ALTER TABLE {$table} ADD MYUPPER CHAR(1)"))
        printf("[006] Cannot add column %s - %s\n",
            var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

    if (!is_object($stmt = $db->query("SELECT id, id AS 'ID_UPPER', label, MiXeD, MYUPPER FROM {$table} ORDER BY id ASC LIMIT 2")))
        printf("[007] %s - %s\n",
            var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

    var_dump($stmt->fetchAll(PDO::FETCH_BOTH));

    if (true !== $db->setAttribute(PDO::ATTR_CASE, PDO::CASE_UPPER))
        printf("[008] Cannot set PDO::ATTR_CASE = PDO::CASE_UPPER %s - %s\n",
            var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

    if (($tmp = $db->getAttribute(PDO::ATTR_CASE)) !== PDO::CASE_UPPER)
        printf("[009] getAttribute(PDO::ATTR_CASE) returns wrong value '%s'\n",
            var_export($tmp, true));

    if (!is_object($stmt = $db->query("SELECT id, label, MiXeD, MYUPPER, MYUPPER AS 'lower' FROM {$table} ORDER BY id ASC LIMIT 1")))
        printf("[010] %s - %s\n",
            var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

    var_dump($stmt->fetchAll(PDO::FETCH_BOTH));

    if (true !== $db->setAttribute(PDO::ATTR_CASE, PDO::CASE_NATURAL))
        printf("[011] Cannot set PDO::ATTR_CASE = PDO::CASE_NATURAL %s - %s\n",
            var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

    if (($tmp = $db->getAttribute(PDO::ATTR_CASE)) !== PDO::CASE_NATURAL)
        printf("[012] getAttribute(PDO::ATTR_CASE) returns wrong value '%s'\n",
            var_export($tmp, true));

    if (!is_object($stmt = $db->query("SELECT id, label, MiXeD, MYUPPER, id AS 'ID' FROM {$table} ORDER BY id ASC LIMIT 1")))
        printf("[013] %s - %s\n",
            var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

    var_dump($stmt->fetchAll(PDO::FETCH_BOTH));

    print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->query('DROP TABLE IF EXISTS pdo_mysql_attr_case');
?>
--EXPECT--
string(15) "PDO::CASE_LOWER"
array(2) {
  [0]=>
  array(6) {
    ["id"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
    ["id_upper"]=>
    string(1) "1"
    [1]=>
    string(1) "1"
    ["label"]=>
    string(1) "a"
    [2]=>
    string(1) "a"
  }
  [1]=>
  array(6) {
    ["id"]=>
    string(1) "2"
    [0]=>
    string(1) "2"
    ["id_upper"]=>
    string(1) "2"
    [1]=>
    string(1) "2"
    ["label"]=>
    string(1) "b"
    [2]=>
    string(1) "b"
  }
}
array(2) {
  [0]=>
  array(10) {
    ["id"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
    ["id_upper"]=>
    string(1) "1"
    [1]=>
    string(1) "1"
    ["label"]=>
    string(1) "a"
    [2]=>
    string(1) "a"
    ["mixed"]=>
    NULL
    [3]=>
    NULL
    ["myupper"]=>
    NULL
    [4]=>
    NULL
  }
  [1]=>
  array(10) {
    ["id"]=>
    string(1) "2"
    [0]=>
    string(1) "2"
    ["id_upper"]=>
    string(1) "2"
    [1]=>
    string(1) "2"
    ["label"]=>
    string(1) "b"
    [2]=>
    string(1) "b"
    ["mixed"]=>
    NULL
    [3]=>
    NULL
    ["myupper"]=>
    NULL
    [4]=>
    NULL
  }
}
array(1) {
  [0]=>
  array(10) {
    ["ID"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
    ["LABEL"]=>
    string(1) "a"
    [1]=>
    string(1) "a"
    ["MIXED"]=>
    NULL
    [2]=>
    NULL
    ["MYUPPER"]=>
    NULL
    [3]=>
    NULL
    ["LOWER"]=>
    NULL
    [4]=>
    NULL
  }
}
array(1) {
  [0]=>
  array(10) {
    ["id"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
    ["label"]=>
    string(1) "a"
    [1]=>
    string(1) "a"
    ["MiXeD"]=>
    NULL
    [2]=>
    NULL
    ["MYUPPER"]=>
    NULL
    [3]=>
    NULL
    ["ID"]=>
    string(1) "1"
    [4]=>
    string(1) "1"
  }
}
done!

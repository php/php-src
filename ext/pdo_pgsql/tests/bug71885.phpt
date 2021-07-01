--TEST--
Request #71855 (PDO placeholder escaping)
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require_once dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
require_once dirname(__FILE__) . '/config.inc';
PDOTest::skip();
?>
--FILE--
<?php
require_once dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
require_once dirname(__FILE__) . '/config.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_NUM);

foreach ([false, true] as $emulate) {
    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, $emulate);

    try {
        $stmt = $db->prepare('select ?- lseg \'((-1,0),(1,0))\'');
        $stmt->execute();
    } catch (PDOException $e) {
        var_dump('ERR');
    }

    $stmt = $db->prepare('select ??- lseg \'((-1,0),(1,0))\'');
    $stmt->execute();

    var_dump($stmt->fetch());
}

?>
==OK==
--EXPECT--
string(3) "ERR"
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
==OK==

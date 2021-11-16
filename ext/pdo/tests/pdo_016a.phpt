--TEST--
PDO Common: PDO::FETCH_BOUND w/o :
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
if (!strncasecmp(getenv('PDOTEST_DSN'), 'oci', strlen('oci'))) die('skip not relevant for oci driver - cannot reexecute after closing cursors without reparse');
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

if ($db->getAttribute(PDO::ATTR_DRIVER_NAME) == 'mysql') {
    $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, true);
}

$db->exec('CREATE TABLE test(idx int NOT NULL PRIMARY KEY, txt VARCHAR(20))');
$db->exec('INSERT INTO test VALUES(0, \'String0\')');
$db->exec('INSERT INTO test VALUES(1, \'String1\')');
$db->exec('INSERT INTO test VALUES(2, \'String2\')');

$stmt1 = $db->prepare('SELECT COUNT(idx) FROM test');
$stmt2 = $db->prepare('SELECT idx, txt FROM test ORDER by idx');

$stmt1->execute();
var_dump($stmt1->fetchColumn());
$stmt1 = null;

$stmt2->execute();
$cont = $stmt2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE);
var_dump($cont);

echo "===WHILE===\n";

$stmt2->bindColumn('idx', $idx);
$stmt2->bindColumn('txt', $txt);
$stmt2->execute();

while($stmt2->fetch(PDO::FETCH_BOUND)) {
    var_dump(array($idx=>$txt));
}

echo "===ALONE===\n";

$stmt3 = $db->prepare('SELECT txt FROM test WHERE idx=:inp');
$stmt3->bindParam('inp', $idx); /* by foreign name */

$stmt4 = $db->prepare('SELECT idx FROM test WHERE txt=:txt');
$stmt4->bindParam('txt', $txt);  /* using same name */

foreach($cont as $idx => $txt)
{
    var_dump(array($idx=>$txt));
    var_dump($stmt3->execute());

    if ($idx == 0) {
        /* portability-wise, you may only bindColumn()s
         * after execute() has been called at least once */
        $stmt3->bindColumn('txt', $col1);
    }
    var_dump($stmt3->fetch(PDO::FETCH_BOUND));
    $stmt3->closeCursor();

    var_dump($stmt4->execute());
    if ($idx == 0) {
        /* portability-wise, you may only bindColumn()s
         * after execute() has been called at least once */
        $stmt4->bindColumn('idx', $col2);
    }
    var_dump($stmt4->fetch(PDO::FETCH_BOUND));
    $stmt4->closeCursor();
    var_dump(array($col2=>$col1));
}

echo "===REBIND/SAME===\n";

$stmt4->bindColumn('idx', $col1);

foreach($cont as $idx => $txt)
{
    var_dump(array($idx=>$txt));
    var_dump($stmt3->execute());
    var_dump($stmt3->fetch(PDO::FETCH_BOUND));
    $stmt3->closeCursor();
    var_dump($col1);
    var_dump($stmt4->execute());
    var_dump($stmt4->fetch(PDO::FETCH_BOUND));
    $stmt4->closeCursor();
    var_dump($col1);
}

echo "===REBIND/CONFLICT===\n";

$stmt2->bindColumn('idx', $col1);
$stmt2->bindColumn('txt', $col1);
$stmt2->execute();

while($stmt2->fetch(PDO::FETCH_BOUND))
{
    var_dump($col1);
}


?>
--EXPECT--
string(1) "3"
array(3) {
  [0]=>
  string(7) "String0"
  [1]=>
  string(7) "String1"
  [2]=>
  string(7) "String2"
}
===WHILE===
array(1) {
  [0]=>
  string(7) "String0"
}
array(1) {
  [1]=>
  string(7) "String1"
}
array(1) {
  [2]=>
  string(7) "String2"
}
===ALONE===
array(1) {
  [0]=>
  string(7) "String0"
}
bool(true)
bool(true)
bool(true)
bool(true)
array(1) {
  [0]=>
  string(7) "String0"
}
array(1) {
  [1]=>
  string(7) "String1"
}
bool(true)
bool(true)
bool(true)
bool(true)
array(1) {
  [1]=>
  string(7) "String1"
}
array(1) {
  [2]=>
  string(7) "String2"
}
bool(true)
bool(true)
bool(true)
bool(true)
array(1) {
  [2]=>
  string(7) "String2"
}
===REBIND/SAME===
array(1) {
  [0]=>
  string(7) "String0"
}
bool(true)
bool(true)
string(7) "String0"
bool(true)
bool(true)
string(1) "0"
array(1) {
  [1]=>
  string(7) "String1"
}
bool(true)
bool(true)
string(7) "String1"
bool(true)
bool(true)
string(1) "1"
array(1) {
  [2]=>
  string(7) "String2"
}
bool(true)
bool(true)
string(7) "String2"
bool(true)
bool(true)
string(1) "2"
===REBIND/CONFLICT===
string(7) "String0"
string(7) "String1"
string(7) "String2"

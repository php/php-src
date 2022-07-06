--TEST--
PDO OCI: Test to verify all implicitly created temporary LOB are cleaned up
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(__DIR__.'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?PHP

require('ext/pdo/tests/pdo_test.inc');
$db = PDOTest::test_factory('ext/pdo_oci/tests/common.phpt');

$clobquery1 = "select TO_CLOB('Hello World') CLOB_DATA from dual";
$clobquery2 = "select TO_CLOB('Hello World') CLOB_DATA from dual";
$clobquery3 = "select TO_CLOB('Hello World') CLOB_DATA from dual";
$clobquery4 = "select TO_CLOB('Hello World') CLOB_DATA from dual";
$clobquery5 = "select TO_CLOB('Hello World') CLOB_DATA from dual";
$clobquery6 = "select TO_CLOB('Hello World') CLOB_DATA from dual";
$clobquery7 = "select TO_CLOB('Hello World') CLOB_DATA from dual";
$clobquery8 = "select TO_CLOB('Hello World') CLOB_DATA from dual";
$clobquery9 = "select TO_CLOB('Hello World') CLOB_DATA from dual";
$clobquery10 = "select TO_CLOB('Hello World') CLOB_DATA from dual";

$stmt= $db->prepare($clobquery1);
$stmt->execute();
$row = $stmt->fetch();
$stmt= $db->prepare($clobquery2);
$stmt->execute();
$row = $stmt->fetch();
$stmt= $db->prepare($clobquery3);
$stmt->execute();
$row = $stmt->fetch();
$stmt= $db->prepare($clobquery4);
$stmt->execute();
$row = $stmt->fetch();
$stmt= $db->prepare($clobquery5);
$stmt->execute();
$row = $stmt->fetch();
$stmt= $db->prepare($clobquery6);
$stmt->execute();
$row = $stmt->fetch();
$stmt= $db->prepare($clobquery7);
$stmt->execute();
$row = $stmt->fetch();
$stmt= $db->prepare($clobquery8);
$stmt->execute();
$row = $stmt->fetch();
$stmt= $db->prepare($clobquery9);
$stmt->execute();
$row = $stmt->fetch();
$stmt= $db->prepare($clobquery10);
$stmt->execute();
$row = $stmt->fetch();

$query1 = "SELECT SYS_CONTEXT('USERENV', 'SID') SID FROM DUAL";

$stmt1 = $db->prepare($query1);
$stmt1->execute();

$row1 = $stmt1->fetch();
$sid_value = $row1[0];

$query2 = "SELECT (CACHE_LOBS+NOCACHE_LOBS+ABSTRACT_LOBS) FROM V\$TEMPORARY_LOBS WHERE SID = :SID_VALUE";

$stmt2 = $db->prepare($query2);
$stmt2->bindParam(':SID_VALUE', $sid_value);
$stmt2->execute();

$row2 = $stmt2->fetch();
/* 1 temporary LOB still exists in V$TEMPORARY_LOBS since the destructor of $stmt is not yet called by PHP */
if ($row2[0] > 1)
{
  echo "TEMP_LOB is not yet cleared!" . $row2[0] . "\n";
}
else
{
  echo "Success! All the temporary LOB in previously closed statements are properly cleaned.\n";
}

?>
--EXPECT--
Success! All the temporary LOB in previously closed statements are properly cleaned.

--TEST--
PDO_OCI: Attribute: Oracle Nulls
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(dirname(__FILE__).'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

require(dirname(__FILE__) . '/../../pdo/tests/pdo_test.inc');

function do_query($dbh)
{
	var_dump($dbh->getAttribute(PDO::ATTR_ORACLE_NULLS));
	$s = $dbh->prepare("select '' as myempty, null as mynull from dual");
	$s->execute();
	while ($r = $s->fetch()) {
		var_dump($r[0]);
		var_dump($r[1]);
	}
}

$dbh = PDOTest::factory();

print "PDO::ATTR_ORACLE_NULLS: Default: ";
do_query($dbh);

print "PDO::ATTR_ORACLE_NULLS: PDO::NULL_NATURAL: ";
$dbh->setAttribute(PDO::ATTR_ORACLE_NULLS, PDO::NULL_NATURAL); // No conversion.

do_query($dbh);

print "PDO::ATTR_ORACLE_NULLS: PDO::NULL_EMPTY_STRING: ";
$dbh->setAttribute(PDO::ATTR_ORACLE_NULLS, PDO::NULL_EMPTY_STRING); // Empty string is converted to NULL.

do_query($dbh);

print "PDO::ATTR_ORACLE_NULLS: PDO::NULL_TO_STRING: ";
$dbh->setAttribute(PDO::ATTR_ORACLE_NULLS, PDO::NULL_TO_STRING); // NULL is converted to an empty string.

do_query($dbh);

echo "Done\n";

?>
--EXPECT--
PDO::ATTR_ORACLE_NULLS: Default: int(0)
NULL
NULL
PDO::ATTR_ORACLE_NULLS: PDO::NULL_NATURAL: int(0)
NULL
NULL
PDO::ATTR_ORACLE_NULLS: PDO::NULL_EMPTY_STRING: int(1)
NULL
NULL
PDO::ATTR_ORACLE_NULLS: PDO::NULL_TO_STRING: int(2)
string(0) ""
string(0) ""
Done
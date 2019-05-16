--TEST--
Bug #54929 (Parse error with single quote in sql comment (pdo-mysql))
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();

?>
--FILE--
<?php

require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$pdodb = PDOTest::test_factory(__DIR__ . '/common.phpt');


function testQuery($query) {
	global $pdodb;
	$stmt = $pdodb->prepare($query);

	if (!$stmt->execute(array("foo"))) {
		var_dump($stmt->errorInfo());
	} else{
		var_dump($stmt->fetch(PDO::FETCH_ASSOC));
	}
}

testQuery("/* ' */ select ? as f1 /* ' */");
testQuery("/* '-- */ select ? as f1 /* *' */");
testQuery("/* ' */ select ? as f1 --';");
testQuery("/* ' */ select ? as f1 -- 'a;");
testQuery("/*'**/ select ? as f1 /* ' */");
testQuery("/*'***/ select ? as f1 /* ' */");
testQuery("/*'**a ***b / ****
******
**/ select ? as f1 /* ' */");

?>
--EXPECTF--
array(1) {
  ["f1"]=>
  string(3) "foo"
}
array(1) {
  ["f1"]=>
  string(3) "foo"
}

Warning: PDOStatement::execute(): SQLSTATE[42000]: Syntax error or access violation: 1064 You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near '--'' at line 1 in %s on line %d
array(3) {
  [0]=>
  string(5) "42000"
  [1]=>
  int(1064)
  [2]=>
  string(%d) "You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near '--'' at line 1"
}
array(1) {
  ["f1"]=>
  string(3) "foo"
}
array(1) {
  ["f1"]=>
  string(3) "foo"
}
array(1) {
  ["f1"]=>
  string(3) "foo"
}
array(1) {
  ["f1"]=>
  string(3) "foo"
}

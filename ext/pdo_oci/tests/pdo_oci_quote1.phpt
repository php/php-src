--TEST--
Test PDO->quote() for PDO_OCI
--SKIPIF--
<?php
die('skip triggers query errors');
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(dirname(__FILE__).'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

require dirname(__FILE__) . '/../../pdo/tests/pdo_test.inc';
$db = PDOTest::factory();

@$db->exec("drop table poq_tab");
$db->query("create table poq_tab (t varchar2(100))");
$stmt = $db->prepare('select * from poq_tab');

// The intent is that the fetched data be identical to the unquoted string.
// Remember!: use bind variables instead of PDO->quote()

$a = array(null, "", "a", "ab", "abc", "ab'cd", "a\b\n", "'", "''", "a'", "'z", "a''b", '"');
foreach ($a as $u) {
	$q = $db->quote($u);
	echo "Unquoted : ";
	var_dump($u);
	echo "Quoted   : ";
	var_dump($q);

	$db->exec("delete from poq_tab");

	$db->query("insert into poq_tab (t) values($q)");
	$stmt->execute();
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
}

echo "Done\n";

@$db->exec("drop table poq_tab");

?>
--EXPECTF--
Unquoted : NULL
Quoted   : unicode(2) "''"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    NULL
  }
}
Unquoted : unicode(0) ""
Quoted   : unicode(2) "''"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    NULL
  }
}
Unquoted : unicode(1) "a"
Quoted   : unicode(3) "'a'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    unicode(1) "a"
  }
}
Unquoted : unicode(2) "ab"
Quoted   : unicode(4) "'ab'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    unicode(2) "ab"
  }
}
Unquoted : unicode(3) "abc"
Quoted   : unicode(5) "'abc'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    unicode(3) "abc"
  }
}
Unquoted : unicode(5) "ab'cd"
Quoted   : unicode(8) "'ab''cd'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    unicode(5) "ab'cd"
  }
}
Unquoted : unicode(4) "a\b
"
Quoted   : unicode(6) "'a\b
'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    unicode(4) "a\b
"
  }
}
Unquoted : unicode(1) "'"
Quoted   : unicode(4) "''''"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    unicode(1) "'"
  }
}
Unquoted : unicode(2) "''"
Quoted   : unicode(6) "''''''"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    unicode(2) "''"
  }
}
Unquoted : unicode(2) "a'"
Quoted   : unicode(5) "'a'''"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    unicode(2) "a'"
  }
}
Unquoted : unicode(2) "'z"
Quoted   : unicode(5) "'''z'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    unicode(2) "'z"
  }
}
Unquoted : unicode(4) "a''b"
Quoted   : unicode(8) "'a''''b'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    unicode(4) "a''b"
  }
}
Unquoted : unicode(1) """
Quoted   : unicode(3) "'"'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    unicode(1) """
  }
}
Done

--TEST--
Test PDO->quote() for PDO_OCI
--EXTENSIONS--
pdo
pdo_oci
--SKIPIF--
<?php
require(__DIR__.'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

require __DIR__ . '/../../pdo/tests/pdo_test.inc';
$db = PDOTest::factory();

$db->query("create table test_pdo_oci_quote1 (t varchar2(100))");
$stmt = $db->prepare('select * from test_pdo_oci_quote1');

// The intent is that the fetched data be identical to the unquoted string.
// Remember!: use bind variables instead of PDO->quote()

$a = array("", "a", "ab", "abc", "ab'cd", "a\b\n", "'", "''", "a'", "'z", "a''b", '"');
foreach ($a as $u) {
    $q = $db->quote($u);
    echo "Unquoted : ";
    var_dump($u);
    echo "Quoted   : ";
    var_dump($q);

    $db->exec("delete from test_pdo_oci_quote1");

    $db->query("insert into test_pdo_oci_quote1 (t) values($q)");
    $stmt->execute();
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
}

echo "Done\n";
?>
--CLEAN--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_oci/tests/common.phpt');
$db->exec("DROP TABLE test_pdo_oci_quote1");
?>
--EXPECT--
Unquoted : string(0) ""
Quoted   : string(2) "''"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    NULL
  }
}
Unquoted : string(1) "a"
Quoted   : string(3) "'a'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    string(1) "a"
  }
}
Unquoted : string(2) "ab"
Quoted   : string(4) "'ab'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    string(2) "ab"
  }
}
Unquoted : string(3) "abc"
Quoted   : string(5) "'abc'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    string(3) "abc"
  }
}
Unquoted : string(5) "ab'cd"
Quoted   : string(8) "'ab''cd'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    string(5) "ab'cd"
  }
}
Unquoted : string(4) "a\b
"
Quoted   : string(6) "'a\b
'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    string(4) "a\b
"
  }
}
Unquoted : string(1) "'"
Quoted   : string(4) "''''"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    string(1) "'"
  }
}
Unquoted : string(2) "''"
Quoted   : string(6) "''''''"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    string(2) "''"
  }
}
Unquoted : string(2) "a'"
Quoted   : string(5) "'a'''"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    string(2) "a'"
  }
}
Unquoted : string(2) "'z"
Quoted   : string(5) "'''z'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    string(2) "'z"
  }
}
Unquoted : string(4) "a''b"
Quoted   : string(8) "'a''''b'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    string(4) "a''b"
  }
}
Unquoted : string(1) """
Quoted   : string(3) "'"'"
array(1) {
  [0]=>
  array(1) {
    ["t"]=>
    string(1) """
  }
}
Done

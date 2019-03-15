--TEST--
oci_fetch_all() - all combinations of flags
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table fetch_all3_tab",
    "create table fetch_all3_tab (id number, value number)",
);

oci8_test_sql_execute($c, $stmtarray);

$insert_sql = "insert into fetch_all3_tab (id, value) values (:idbv,:vbv)";
$s = oci_parse($c, $insert_sql);
oci_bind_by_name($s, ":idbv", $idbv, -1, SQLT_INT);
oci_bind_by_name($s, ":vbv", $vbv, -1, SQLT_INT);

for ($i = 1; $i <= 4; $i++) {
    $idbv = $i;
    $vbv = -$i;
    oci_execute($s, OCI_DEFAULT);
}

oci_commit($c);

// Run Test

$select_sql = "select id, value from fetch_all3_tab order by id";

$s = oci_parse($c, $select_sql);

echo "None\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1));
var_dump($all);

echo "OCI_ASSOC\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_ASSOC));
var_dump($all);

echo "OCI_FETCHSTATEMENT_BY_COLUMN\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_FETCHSTATEMENT_BY_COLUMN));
var_dump($all);

echo "OCI_FETCHSTATEMENT_BY_COLUMN|OCI_ASSOC\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_FETCHSTATEMENT_BY_COLUMN|OCI_ASSOC));
var_dump($all);

echo "OCI_FETCHSTATEMENT_BY_COLUMN|OCI_NUM\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_FETCHSTATEMENT_BY_COLUMN|OCI_NUM));
var_dump($all);

echo "OCI_FETCHSTATEMENT_BY_COLUMN|OCI_NUM|OCI_ASSOC\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_FETCHSTATEMENT_BY_COLUMN|OCI_NUM|OCI_ASSOC));
var_dump($all);

echo "OCI_FETCHSTATEMENT_BY_ROW\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_FETCHSTATEMENT_BY_ROW));
var_dump($all);

echo "OCI_FETCHSTATEMENT_BY_ROW|OCI_ASSOC\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_FETCHSTATEMENT_BY_ROW|OCI_ASSOC));
var_dump($all);

echo "OCI_FETCHSTATEMENT_BY_ROW|OCI_FETCHSTATEMENT_BY_COLUMN\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_FETCHSTATEMENT_BY_ROW|OCI_FETCHSTATEMENT_BY_COLUMN));
var_dump($all);

echo "OCI_FETCHSTATEMENT_BY_ROW|OCI_FETCHSTATEMENT_BY_COLUMN|OCI_ASSOC\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_FETCHSTATEMENT_BY_ROW|OCI_FETCHSTATEMENT_BY_COLUMN|OCI_ASSOC));
var_dump($all);

echo "OCI_FETCHSTATEMENT_BY_ROW|OCI_FETCHSTATEMENT_BY_COLUMN|OCI_NUM\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_FETCHSTATEMENT_BY_ROW|OCI_FETCHSTATEMENT_BY_COLUMN|OCI_NUM));
var_dump($all);

echo "OCI_FETCHSTATEMENT_BY_ROW|OCI_FETCHSTATEMENT_BY_COLUMN|OCI_NUM|OCI_ASSOC\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_FETCHSTATEMENT_BY_ROW|OCI_FETCHSTATEMENT_BY_COLUMN|OCI_NUM|OCI_ASSOC));
var_dump($all);

echo "OCI_FETCHSTATEMENT_BY_ROW|OCI_NUM\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_FETCHSTATEMENT_BY_ROW|OCI_NUM));
var_dump($all);

echo "OCI_FETCHSTATEMENT_BY_ROW|OCI_NUM|OCI_ASSOC\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_FETCHSTATEMENT_BY_ROW|OCI_NUM|OCI_ASSOC));
var_dump($all);

echo "OCI_NUM\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_NUM));
var_dump($all);

echo "OCI_NUM|OCI_ASSOC\n";
oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, -1, OCI_NUM|OCI_ASSOC));
var_dump($all);

// Cleanup

$stmtarray = array(
    "drop table fetch_all3_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";
?>
--EXPECT--
None
int(4)
array(2) {
  ["ID"]=>
  array(4) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
    [3]=>
    string(1) "4"
  }
  ["VALUE"]=>
  array(4) {
    [0]=>
    string(2) "-1"
    [1]=>
    string(2) "-2"
    [2]=>
    string(2) "-3"
    [3]=>
    string(2) "-4"
  }
}
OCI_ASSOC
int(4)
array(2) {
  ["ID"]=>
  array(4) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
    [3]=>
    string(1) "4"
  }
  ["VALUE"]=>
  array(4) {
    [0]=>
    string(2) "-1"
    [1]=>
    string(2) "-2"
    [2]=>
    string(2) "-3"
    [3]=>
    string(2) "-4"
  }
}
OCI_FETCHSTATEMENT_BY_COLUMN
int(4)
array(2) {
  ["ID"]=>
  array(4) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
    [3]=>
    string(1) "4"
  }
  ["VALUE"]=>
  array(4) {
    [0]=>
    string(2) "-1"
    [1]=>
    string(2) "-2"
    [2]=>
    string(2) "-3"
    [3]=>
    string(2) "-4"
  }
}
OCI_FETCHSTATEMENT_BY_COLUMN|OCI_ASSOC
int(4)
array(2) {
  ["ID"]=>
  array(4) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
    [3]=>
    string(1) "4"
  }
  ["VALUE"]=>
  array(4) {
    [0]=>
    string(2) "-1"
    [1]=>
    string(2) "-2"
    [2]=>
    string(2) "-3"
    [3]=>
    string(2) "-4"
  }
}
OCI_FETCHSTATEMENT_BY_COLUMN|OCI_NUM
int(4)
array(2) {
  [0]=>
  array(4) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
    [3]=>
    string(1) "4"
  }
  [1]=>
  array(4) {
    [0]=>
    string(2) "-1"
    [1]=>
    string(2) "-2"
    [2]=>
    string(2) "-3"
    [3]=>
    string(2) "-4"
  }
}
OCI_FETCHSTATEMENT_BY_COLUMN|OCI_NUM|OCI_ASSOC
int(4)
array(2) {
  [0]=>
  array(4) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
    [3]=>
    string(1) "4"
  }
  [1]=>
  array(4) {
    [0]=>
    string(2) "-1"
    [1]=>
    string(2) "-2"
    [2]=>
    string(2) "-3"
    [3]=>
    string(2) "-4"
  }
}
OCI_FETCHSTATEMENT_BY_ROW
int(4)
array(4) {
  [0]=>
  array(2) {
    ["ID"]=>
    string(1) "1"
    ["VALUE"]=>
    string(2) "-1"
  }
  [1]=>
  array(2) {
    ["ID"]=>
    string(1) "2"
    ["VALUE"]=>
    string(2) "-2"
  }
  [2]=>
  array(2) {
    ["ID"]=>
    string(1) "3"
    ["VALUE"]=>
    string(2) "-3"
  }
  [3]=>
  array(2) {
    ["ID"]=>
    string(1) "4"
    ["VALUE"]=>
    string(2) "-4"
  }
}
OCI_FETCHSTATEMENT_BY_ROW|OCI_ASSOC
int(4)
array(4) {
  [0]=>
  array(2) {
    ["ID"]=>
    string(1) "1"
    ["VALUE"]=>
    string(2) "-1"
  }
  [1]=>
  array(2) {
    ["ID"]=>
    string(1) "2"
    ["VALUE"]=>
    string(2) "-2"
  }
  [2]=>
  array(2) {
    ["ID"]=>
    string(1) "3"
    ["VALUE"]=>
    string(2) "-3"
  }
  [3]=>
  array(2) {
    ["ID"]=>
    string(1) "4"
    ["VALUE"]=>
    string(2) "-4"
  }
}
OCI_FETCHSTATEMENT_BY_ROW|OCI_FETCHSTATEMENT_BY_COLUMN
int(4)
array(4) {
  [0]=>
  array(2) {
    ["ID"]=>
    string(1) "1"
    ["VALUE"]=>
    string(2) "-1"
  }
  [1]=>
  array(2) {
    ["ID"]=>
    string(1) "2"
    ["VALUE"]=>
    string(2) "-2"
  }
  [2]=>
  array(2) {
    ["ID"]=>
    string(1) "3"
    ["VALUE"]=>
    string(2) "-3"
  }
  [3]=>
  array(2) {
    ["ID"]=>
    string(1) "4"
    ["VALUE"]=>
    string(2) "-4"
  }
}
OCI_FETCHSTATEMENT_BY_ROW|OCI_FETCHSTATEMENT_BY_COLUMN|OCI_ASSOC
int(4)
array(4) {
  [0]=>
  array(2) {
    ["ID"]=>
    string(1) "1"
    ["VALUE"]=>
    string(2) "-1"
  }
  [1]=>
  array(2) {
    ["ID"]=>
    string(1) "2"
    ["VALUE"]=>
    string(2) "-2"
  }
  [2]=>
  array(2) {
    ["ID"]=>
    string(1) "3"
    ["VALUE"]=>
    string(2) "-3"
  }
  [3]=>
  array(2) {
    ["ID"]=>
    string(1) "4"
    ["VALUE"]=>
    string(2) "-4"
  }
}
OCI_FETCHSTATEMENT_BY_ROW|OCI_FETCHSTATEMENT_BY_COLUMN|OCI_NUM
int(4)
array(4) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(2) "-1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "2"
    [1]=>
    string(2) "-2"
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "3"
    [1]=>
    string(2) "-3"
  }
  [3]=>
  array(2) {
    [0]=>
    string(1) "4"
    [1]=>
    string(2) "-4"
  }
}
OCI_FETCHSTATEMENT_BY_ROW|OCI_FETCHSTATEMENT_BY_COLUMN|OCI_NUM|OCI_ASSOC
int(4)
array(4) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(2) "-1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "2"
    [1]=>
    string(2) "-2"
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "3"
    [1]=>
    string(2) "-3"
  }
  [3]=>
  array(2) {
    [0]=>
    string(1) "4"
    [1]=>
    string(2) "-4"
  }
}
OCI_FETCHSTATEMENT_BY_ROW|OCI_NUM
int(4)
array(4) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(2) "-1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "2"
    [1]=>
    string(2) "-2"
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "3"
    [1]=>
    string(2) "-3"
  }
  [3]=>
  array(2) {
    [0]=>
    string(1) "4"
    [1]=>
    string(2) "-4"
  }
}
OCI_FETCHSTATEMENT_BY_ROW|OCI_NUM|OCI_ASSOC
int(4)
array(4) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(2) "-1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "2"
    [1]=>
    string(2) "-2"
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "3"
    [1]=>
    string(2) "-3"
  }
  [3]=>
  array(2) {
    [0]=>
    string(1) "4"
    [1]=>
    string(2) "-4"
  }
}
OCI_NUM
int(4)
array(2) {
  [0]=>
  array(4) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
    [3]=>
    string(1) "4"
  }
  [1]=>
  array(4) {
    [0]=>
    string(2) "-1"
    [1]=>
    string(2) "-2"
    [2]=>
    string(2) "-3"
    [3]=>
    string(2) "-4"
  }
}
OCI_NUM|OCI_ASSOC
int(4)
array(2) {
  [0]=>
  array(4) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
    [3]=>
    string(1) "4"
  }
  [1]=>
  array(4) {
    [0]=>
    string(2) "-1"
    [1]=>
    string(2) "-2"
    [2]=>
    string(2) "-3"
    [3]=>
    string(2) "-4"
  }
}
Done

--TEST--
Bug #42841 (REF CURSOR and oci_new_cursor PHP crash)
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--INI--
oci8.statement_cache_size=20
--FILE--
<?php

require __DIR__.'/details.inc';

// note a oci_new_connect() occurs lower in the script
$c = oci_connect($user, $password, $dbase);

// Initialization

$stmtarray = array(
    "create or replace procedure bug42841_proc(out_1 out sys_refcursor) is
   begin
      open out_1 for select 11 from dual union all select 12 from dual union all select 13 from dual;
   end bug42841_proc;",

    "create or replace package bug43449_pkg is
        type cursortype is ref Cursor;
        function testcursor return cursortype;
    end bug43449_pkg;",

    "create or replace package body bug43449_pkg is
    function testcursor return cursortype is
    retCursor cursorType;
    begin
        Open retCursor For 'select * from dual';
        return retCursor;
    end;
    end bug43449_pkg;"
);

oci8_test_sql_execute($c, $stmtarray);

// Main code

function do_bug42841($c)
{
    echo "First attempt\n";

    $sql = "BEGIN bug42841_proc(:cursor); END;";
    $stmt = oci_parse($c, $sql);
    $cursor = oci_new_cursor($c);
    oci_bind_by_name($stmt, ":cursor", $cursor, -1, OCI_B_CURSOR);

    oci_execute($stmt, OCI_DEFAULT);
    oci_execute($cursor);

    while($row = oci_fetch_array($cursor, OCI_ASSOC + OCI_RETURN_LOBS)) {
        $data1[] = $row;
    }

    oci_free_statement($stmt);
    oci_free_statement($cursor);
    var_dump($data1);

    echo "Second attempt\n";

    $sql = "BEGIN bug42841_proc(:cursor); END;";
    $stmt = oci_parse($c, $sql);
    $cursor = oci_new_cursor($c);
    oci_bind_by_name($stmt, ":cursor", $cursor, -1, OCI_B_CURSOR);

    oci_execute($stmt, OCI_DEFAULT);
    oci_execute($cursor);

    while($row = oci_fetch_array($cursor, OCI_ASSOC + OCI_RETURN_LOBS)) {
        $data2[] = $row;
    }

    oci_free_statement($stmt);
    oci_free_statement($cursor);
    var_dump($data2);
}

function do_bug43449($c)
{

    for ($i = 0; $i < 2; $i++) {
        var_dump(bug43449_getCur($c));
    }
}

function bug43449_getCur($c)
{
    $cur = oci_new_cursor($c);
    $stmt = oci_parse($c, 'begin :cur := bug43449_pkg.testcursor; end;');
    oci_bind_by_name($stmt, ':cur', $cur, -1, OCI_B_CURSOR);
    oci_execute($stmt, OCI_DEFAULT);
    oci_execute($cur, OCI_DEFAULT);

    $ret = array();

    while ($row = oci_fetch_assoc($cur)) {
        $ret[] = $row;
    }

    oci_free_statement($cur);
    oci_free_statement($stmt);
    return $ret;
}

echo "Test bug 42841: Procedure with OUT cursor parameter\n";
do_bug42841($c);

$c = oci_new_connect($user, $password, $dbase);

echo "Test bug 43449: Cursor as function result\n";
do_bug43449($c);

// Cleanup

$stmtarray = array(
    "drop procedure bug42841_proc",
    "drop package bug43449_pkg"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECT--
Test bug 42841: Procedure with OUT cursor parameter
First attempt
array(3) {
  [0]=>
  array(1) {
    [11]=>
    string(2) "11"
  }
  [1]=>
  array(1) {
    [11]=>
    string(2) "12"
  }
  [2]=>
  array(1) {
    [11]=>
    string(2) "13"
  }
}
Second attempt
array(3) {
  [0]=>
  array(1) {
    [11]=>
    string(2) "11"
  }
  [1]=>
  array(1) {
    [11]=>
    string(2) "12"
  }
  [2]=>
  array(1) {
    [11]=>
    string(2) "13"
  }
}
Test bug 43449: Cursor as function result
array(1) {
  [0]=>
  array(1) {
    ["DUMMY"]=>
    string(1) "X"
  }
}
array(1) {
  [0]=>
  array(1) {
    ["DUMMY"]=>
    string(1) "X"
  }
}
Done

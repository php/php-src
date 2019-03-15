--TEST--
Prefetch with REF cursor. Test No 2
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension");
if (!extension_loaded('oci8')) die("skip no oci8 extension");
require(__DIR__."/connect.inc");
preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) &&
      ($matches[1] >= 10))) {
       	die("skip expected output only valid when using Oracle 10g or greater database server");
}
preg_match('/^([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)/', oci_client_version(), $matches);
if (!(isset($matches[0]) &&
    (($matches[1] == 11 && $matches[2] >= 2) ||
     ($matches[1] >= 12)
     ))) {
    die("skip test expected to work only with Oracle 11gR2 or greater version of client");
}
?>
--FILE--
<?php
require __DIR__."/connect.inc";

// Creates the necessary package and tables.
$stmtarray = array(
	   "DROP TABLE refcurtest",
	   "CREATE TABLE refcurtest (c1 NUMBER, c2 VARCHAR(20))",
           "CREATE or REPLACE PACKAGE refcurpkg is
           type refcursortype is ref cursor;
           procedure open_ref_cur(cur1 out refcursortype);
           procedure fetch_ref_cur(cur1 in refcursortype, c1 out number,c2 out varchar2);
           end refcurpkg;",
          "CREATE or REPLACE PACKAGE body refcurpkg is
  	    procedure open_ref_cur(cur1 out refcursortype) is
              begin
	        open cur1 for select * from refcurtest order by c1;
	      end open_ref_cur;
  	     procedure fetch_ref_cur(cur1 in refcursortype, c1 out number,
		c2 out varchar2) is
  	      begin
	    	fetch cur1 into c1,c2;
	    end fetch_ref_cur;
         end refcurpkg;"
	);

oci8_test_sql_execute($c, $stmtarray);

// Insert 500 rows into the table.
$insert_sql = "INSERT INTO refcurtest (c1, c2) VALUES (:c1,:c2)";
if (!($s = oci_parse($c, $insert_sql))) {
    die("oci_parse(insert) failed!\n");
}

for ($i = 0; $i <= 500; $i++) {
    $val2 = 'test'.$i;
    oci_bind_by_name($s,':c1',$i);
    oci_bind_by_name($s,':c2',$val2);
    if (!oci_execute($s)) {
        die("oci_execute(insert) failed!\n");
    }
}

// Steps to Fetch from PHP . For every sub-test,the cursor is bound and then executed.

$sql1 = "begin refcurpkg.open_ref_cur(:cur1); end;";
$s1 = oci_parse($c,$sql1);
$cur1 = oci_new_cursor($c);
if (!oci_bind_by_name($s1,":cur1",$cur1,-1,SQLT_RSET)) {
    die("oci_bind_by_name(sql1) failed!\n");
}


// Steps to Fetch from PL/SQL . For every sub-test,the cursor is bound and then executed.

$sql2 = "begin refcurpkg.fetch_ref_cur(:curs1,:c1,:c2); end;";
$s2 = oci_parse($c,$sql2);
if (!oci_bind_by_name($s2, ":curs1", $cur1, -1, SQLT_RSET)) {
    die("oci_bind_by_name(sql2) failed!\n");
}
if (!oci_bind_by_name($s2, ":c1", $c1, -1, SQLT_INT)) {
    die("oci_bind_by_name(sql2) failed!\n");
}
if (!oci_bind_by_name($s2, ":c2", $c2, 20, SQLT_CHR)) {
    die("oci_bind_by_name(sql2) failed!\n");
}


echo "------Test 1- Check Roundtrips with  prefetch 0 and 5 -----------\n";
oci_execute($s1);
oci_execute($cur1);
$initial_rt = print_roundtrips($c);
oci_set_prefetch($cur1,0);
for ($i = 0;$i<5;$i++) {
    var_dump(oci_fetch_row($cur1));
}

$cnt = (print_roundtrips($c) - $initial_rt);
echo "Number of roundtrips made with prefetch count 0 for 5 rows is  $cnt\n";

$initial_rt = print_roundtrips($c);
oci_set_prefetch($cur1,5);
for ($i = 0;$i<5;$i++) {
    var_dump(oci_fetch_row($cur1));
}

$cnt = (print_roundtrips($c) - $initial_rt );
echo "Number of roundtrips made with prefetch count 5 for 5 rows is  $cnt\n";

echo "------Test 2 - Set Prefetch before PL/SQL fetch ----------\n";
// Fetch from PHP
$cur1 = oci_new_cursor($c);
if (!oci_bind_by_name($s1,":cur1",$cur1,-1,SQLT_RSET)) {
    die("oci_bind_by_name(sql1) failed!\n");
}

echo "Fetch Row from PHP\n";
oci_execute($s1);
oci_execute($cur1);
var_dump(oci_fetch_row($cur1));
oci_set_prefetch($cur1,5);

// Fetch from PL/SQL
if (!oci_bind_by_name($s2,":curs1",$cur1,-1,SQLT_RSET)) {
    die("oci_bind_by_name(sql2) failed!\n");
}
oci_execute($s2);
echo "Fetch Row from PL/SQL\n";
var_dump($c1);
var_dump($c2);

function  print_roundtrips($c) {
    $sql_stmt = "select value from v\$mystat a,v\$statname c where
	 a.statistic#=c.statistic# and c.name='SQL*Net roundtrips to/from client'";
    $s = oci_parse($c,$sql_stmt);
    oci_define_by_name($s,"VALUE",$value);
    oci_execute($s);
    oci_fetch($s);
    return $value;
}

// Clean up  here

$stmtarray = array(
    "drop package refcurpkg",
    "drop table refcurtest"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";
?>
--EXPECTF--
------Test 1- Check Roundtrips with  prefetch 0 and 5 -----------
array(2) {
  [0]=>
  string(1) "0"
  [1]=>
  string(5) "test0"
}
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(5) "test1"
}
array(2) {
  [0]=>
  string(1) "2"
  [1]=>
  string(5) "test2"
}
array(2) {
  [0]=>
  string(1) "3"
  [1]=>
  string(5) "test3"
}
array(2) {
  [0]=>
  string(1) "4"
  [1]=>
  string(5) "test4"
}
Number of roundtrips made with prefetch count 0 for 5 rows is  6
array(2) {
  [0]=>
  string(1) "5"
  [1]=>
  string(5) "test5"
}
array(2) {
  [0]=>
  string(1) "6"
  [1]=>
  string(5) "test6"
}
array(2) {
  [0]=>
  string(1) "7"
  [1]=>
  string(5) "test7"
}
array(2) {
  [0]=>
  string(1) "8"
  [1]=>
  string(5) "test8"
}
array(2) {
  [0]=>
  string(1) "9"
  [1]=>
  string(5) "test9"
}
Number of roundtrips made with prefetch count 5 for 5 rows is  2
------Test 2 - Set Prefetch before PL/SQL fetch ----------
Fetch Row from PHP
array(2) {
  [0]=>
  string(1) "0"
  [1]=>
  string(5) "test0"
}
Fetch Row from PL/SQL
int(101)
string(%d) "test101"
Done

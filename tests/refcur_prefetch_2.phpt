--TEST--
Prefetch with REF cursor. Test No 2
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension");
if (!extension_loaded('oci8')) die("skip no oci8 extension");
require(dirname(__FILE__)."/connect.inc");
ob_start();
phpinfo(INFO_MODULES);
$phpinfo = ob_get_clean();
$iv = preg_match('/Oracle .*Version => (11\.2|12\.)/', $phpinfo);
if ($iv == 1) {
    $sv = oci_server_version($c);
    $sv = preg_match('/Release 1[012]\./', $sv, $matches);
    if ($sv != 1) {
        die ("skip expected output only valid when using Oracle 10g or greater server");
    }
}
else {
    die ("skip expected output only valid when using  Oracle 11.1 or greater client");
}
?>
--FILE--
<?php
require dirname(__FILE__)."/connect.inc";

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

foreach($stmtarray as $stmt) {
	$s = oci_parse($c,$stmt);
	$r = @oci_execute($s);
    if (!$r) {
		$msg = oci_error($s);
		if ($msg['code'] != 942) {
            echo $msg['message'],"\n";
		}
	}
}

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
if (!oci_bind_by_name($s2,":curs1",$cur1,-1,SQLT_RSET)) {
    die("oci_bind_by_name(sql2) failed!\n");
}
if (!oci_bind_by_name($s2,":c1",$c1,SQLT_INT)) {
    die("oci_bind_by_name(sql2) failed!\n");
}
if (!oci_bind_by_name($s2,":c2",$c2,SQLT_AFC)) {
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

echo "Fetch Row  from PHP\n";
oci_execute($s1);
oci_execute($cur1);
var_dump(oci_fetch_row($cur1));
oci_set_prefetch($cur1,5);

// Fetch from PL/SQL 
if (!oci_bind_by_name($s2,":curs1",$cur1,-1,SQLT_RSET)) {
    die("oci_bind_by_name(sql2) failed!\n");
}
oci_execute($s2);
echo "Fetch Row  from PL/SQL\n";
var_dump($c1);
var_dump($c2);

echo "------Test 3 - Set Prefetch after PL/SQL fetch ----------\n";
$cur1 = oci_new_cursor($c);
// Fetch from PL/SQL 
if (!oci_bind_by_name($s2,":curs1",$cur1,-1,SQLT_RSET)) {
    die("oci_bind_by_name(sql2) failed!\n");
}
oci_execute($s2);
echo "Fetch Row  from PL/SQL\n";
var_dump($c1);
var_dump($c2);

// Fetch from PHP
echo "Fetch Row  from PHP\n";
if (!oci_bind_by_name($s1,":cur1",$cur1,-1,SQLT_RSET)) {
    die("oci_bind_by_name(sql1) failed!\n");
}
oci_set_prefetch($cur1,5);
oci_execute($s1);
oci_execute($cur1);
var_dump(oci_fetch_row($cur1));

echo "------Test 4- Overwrite prefetch-----------\n";
// Fetch from PHP
$cur1 = oci_new_cursor($c);
if (!oci_bind_by_name($s1,":cur1",$cur1,-1,SQLT_RSET)) {
    die("oci_bind_by_name(sql1) failed!\n");
}
echo "Fetch Row  from PHP\n";
oci_execute($s1);
oci_execute($cur1);
var_dump(oci_fetch_row($cur1));
oci_set_prefetch($cur1,5);
oci_set_prefetch($cur1,0);
oci_set_prefetch($cur1,100);

// Fetch from PL/SQL
if (!oci_bind_by_name($s2,":curs1",$cur1,-1,SQLT_RSET)) {
    die("oci_bind_by_name(sql2) failed!\n");
}
oci_execute($s2);
echo "Fetch Row  from PL/SQL\n";
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

foreach($stmtarray as $stmt) {
    $s = oci_parse($c,$stmt);
    $r = @oci_execute($s);
    if (!$r) {
        $msg = oci_error($s);
        echo $msg['message'],"\n";
    }
}

oci_close($c);
echo "Done\n";
?>
--EXPECTF--
------Test 1- Check Roundtrips with  prefetch 0 and 5 -----------
array(2) {
  [0]=>
  %unicode|string%(%d) "0"
  [1]=>
  %unicode|string%(%d) "test0"
}
array(2) {
  [0]=>
  %unicode|string%(%d) "1"
  [1]=>
  %unicode|string%(%d) "test1"
}
array(2) {
  [0]=>
  %unicode|string%(%d) "2"
  [1]=>
  %unicode|string%(%d) "test2"
}
array(2) {
  [0]=>
  %unicode|string%(%d) "3"
  [1]=>
  %unicode|string%(%d) "test3"
}
array(2) {
  [0]=>
  %unicode|string%(%d) "4"
  [1]=>
  %unicode|string%(%d) "test4"
}
Number of roundtrips made with prefetch count 0 for 5 rows is  6
array(2) {
  [0]=>
  %unicode|string%(%d) "5"
  [1]=>
  %unicode|string%(%d) "test5"
}
array(2) {
  [0]=>
  %unicode|string%(%d) "6"
  [1]=>
  %unicode|string%(%d) "test6"
}
array(2) {
  [0]=>
  %unicode|string%(%d) "7"
  [1]=>
  %unicode|string%(%d) "test7"
}
array(2) {
  [0]=>
  %unicode|string%(%d) "8"
  [1]=>
  %unicode|string%(%d) "test8"
}
array(2) {
  [0]=>
  %unicode|string%(%d) "9"
  [1]=>
  %unicode|string%(%d) "test9"
}
Number of roundtrips made with prefetch count 5 for 5 rows is  2
------Test 2 - Set Prefetch before PL/SQL fetch ----------
Fetch Row  from PHP
array(2) {
  [0]=>
  %unicode|string%(%d) "0"
  [1]=>
  %unicode|string%(%d) "test0"
}
Fetch Row  from PL/SQL
%unicode|string%(%d) "101"
%unicode|string%(%d) "test101"
------Test 3 - Set Prefetch after PL/SQL fetch ----------

Warning: oci_execute(): ORA-01001: %s
ORA-06512: at "SYSTEM.REFCURPKG", line %d
ORA-06512: at line %d in %s on line %d
Fetch Row  from PL/SQL
%unicode|string%(%d) "101"
%unicode|string%(%d) "test101"
Fetch Row  from PHP
array(2) {
  [0]=>
  %unicode|string%(%d) "0"
  [1]=>
  %unicode|string%(%d) "test0"
}
------Test 4- Overwrite prefetch-----------
Fetch Row  from PHP
array(2) {
  [0]=>
  %unicode|string%(%d) "0"
  [1]=>
  %unicode|string%(%d) "test0"
}
Fetch Row  from PL/SQL
%unicode|string%(%d) "101"
%unicode|string%(%d) "test101"
Done

--TEST--
Basic test for setting Oracle 11gR2 "edition" attribute
--SKIPIF--
<?php 
if (!extension_loaded('oci8')) die("skip no oci8 extension"); 
require(dirname(__FILE__)."/connect.inc");
if (strcasecmp($user, "system") && strcasecmp($user, "sys")) {
    die("skip needs to be run as a DBA user");
}
if ($test_drcp) {
    die("skip as Output might vary with DRCP");
}
preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) && 
      (($matches[1] == 11 && $matches[2] >= 2) ||
       ($matches[1] >= 12)
       ))) {
       	die("skip expected output only valid when using Oracle 11gR2 or greater database server");
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

/* In 11.2, there can only be one child edition.  So this test will
 * fail to create the necessary editions if a child edition exists
 * already 
 */

$testuser     = 'testuser_attr_1';  // Used in conn_attr.inc
$testpassword = 'testuser'; 

require(dirname(__FILE__)."/conn_attr.inc");

function select_fn($conn) {
	$s = oci_parse($conn,"select * from view_ed");
	oci_execute($s);
	while ($row = oci_fetch_row($s)) {
		var_dump($row);
	}
}
/* Create a editon MYEDITION 
   create a view view_ed in MYEDITION1.
   create the same view 'view_ed' with a different definition in MYEDITION.
   select from both the editions and verify the contents. */

set_edit_attr('MYEDITION');
$conn = oci_connect($testuser,$testpassword,$dbase); 
if ($conn === false) {
    $m = oci_error();
    die("Error:" . $m['message']);
}
    
$stmtarray = array(
    "drop table edit_tab",
    "create table edit_tab (name varchar2(10),age number,job varchar2(50), salary number)",
    "insert into edit_tab values('mike',30,'Senior engineer',200)",
    "insert into edit_tab values('juan',25,'engineer',100)",
    "create or replace editioning view view_ed as select name,age,job from edit_tab",
);

oci8_test_sql_execute($conn, $stmtarray);

// Check the current edition of the DB and the contents of view_ed.
get_edit_attr($conn);
select_fn($conn);

// Create a different version of view_ed in MYEDITION1.
set_edit_attr('MYEDITION1');
$conn2 = oci_new_connect($testuser,$testpassword,$dbase); 
$stmt = "create or replace editioning view view_ed as select name,age,job,salary from edit_tab";
$s = oci_parse($conn2, $stmt);
oci_execute($s);

// Check the current edition of the DB and the contents of view_ed.
get_edit_attr($conn2);
select_fn($conn2);

// Verify the contents in MYEDITION EDITION.
echo "version of view_ed in MYEDITION \n";
get_edit_attr($conn);
select_fn($conn);

clean_up($c);

oci_close($conn);
oci_close($conn2);
echo "Done\n";

?>
--EXPECTF--
The value of edition has been successfully set
The value of current EDITION is MYEDITION
array(3) {
  [0]=>
  string(%d) "mike"
  [1]=>
  string(%d) "30"
  [2]=>
  string(%d) "Senior engineer"
}
array(3) {
  [0]=>
  string(%d) "juan"
  [1]=>
  string(%d) "25"
  [2]=>
  string(%d) "engineer"
}
 The value of edition has been successfully set
The value of current EDITION is MYEDITION1
array(4) {
  [0]=>
  string(%d) "mike"
  [1]=>
  string(%d) "30"
  [2]=>
  string(%d) "Senior engineer"
  [3]=>
  string(%d) "200"
}
array(4) {
  [0]=>
  string(%d) "juan"
  [1]=>
  string(%d) "25"
  [2]=>
  string(%d) "engineer"
  [3]=>
  string(%d) "100"
}
version of view_ed in MYEDITION 
The value of current EDITION is MYEDITION
array(3) {
  [0]=>
  string(%d) "mike"
  [1]=>
  string(%d) "30"
  [2]=>
  string(%d) "Senior engineer"
}
array(3) {
  [0]=>
  string(%d) "juan"
  [1]=>
  string(%d) "25"
  [2]=>
  string(%d) "engineer"
}
Done

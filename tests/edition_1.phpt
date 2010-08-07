--TEST--
Basic test for setting Oracle 11gR2 "edition" attribute
--SKIPIF--
<?php 
if (!extension_loaded('oci8')) die("skip no oci8 extension"); 
require(dirname(__FILE__)."/connect.inc");
if (strcasecmp($user, "system") && strcasecmp($user, "sys")) 
    die("skip needs to be run as a DBA user");
if ($test_drcp) 
    die("skip as Output might vary with DRCP");

$sv = oci_server_version($c);
$sv = preg_match('/Release (11\.2|12)/', $sv, $matches);
if ($sv == 1) {
    ob_start();
    phpinfo(INFO_MODULES);
    $phpinfo = ob_get_clean();
    $iv = preg_match('/Oracle .*Version => (11\.2|12)/', $phpinfo);
    if ($iv != 1) {
        die ("skip tests a feature that works only with Oracle 11gR2 or greater version of client");
    }
}
else {
    die ("skip tests a feature that works only with Oracle 11gR2 or greater version of server");
}

?>
--FILE--
<?php

/* In 11.2, there can only be one child edition.  So this test will
 * fail to create the necessary editions if a child edition exists
 * already 
 */

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
$conn = oci_connect('testuser','testuser',$dbase); 
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

foreach ($stmtarray as $stmt) {
    $s = oci_parse($conn, $stmt);
    @oci_execute($s);
}

// Check the current edition of the DB and the contents of view_ed.
get_edit_attr($conn);
select_fn($conn);

// Create a different version of view_ed in MYEDITION1.
set_edit_attr('MYEDITION1');
$conn2 = oci_new_connect('testuser','testuser',$dbase); 
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
  %unicode|string%(%d) "mike"
  [1]=>
  %unicode|string%(%d) "30"
  [2]=>
  %unicode|string%(%d) "Senior engineer"
}
array(3) {
  [0]=>
  %unicode|string%(%d) "juan"
  [1]=>
  %unicode|string%(%d) "25"
  [2]=>
  %unicode|string%(%d) "engineer"
}
 The value of edition has been successfully set
The value of current EDITION is MYEDITION1
array(4) {
  [0]=>
  %unicode|string%(%d) "mike"
  [1]=>
  %unicode|string%(%d) "30"
  [2]=>
  %unicode|string%(%d) "Senior engineer"
  [3]=>
  %unicode|string%(%d) "200"
}
array(4) {
  [0]=>
  %unicode|string%(%d) "juan"
  [1]=>
  %unicode|string%(%d) "25"
  [2]=>
  %unicode|string%(%d) "engineer"
  [3]=>
  %unicode|string%(%d) "100"
}
version of view_ed in MYEDITION 
The value of current EDITION is MYEDITION
array(3) {
  [0]=>
  %unicode|string%(%d) "mike"
  [1]=>
  %unicode|string%(%d) "30"
  [2]=>
  %unicode|string%(%d) "Senior engineer"
}
array(3) {
  [0]=>
  %unicode|string%(%d) "juan"
  [1]=>
  %unicode|string%(%d) "25"
  [2]=>
  %unicode|string%(%d) "engineer"
}
Done

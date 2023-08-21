--TEST--
Set and check Oracle 11gR2 "edition" attribute
--EXTENSIONS--
oci8
--SKIPIF--
<?php
if (getenv('SKIP_REPEAT')) die('skip fails with repeat');
require_once 'skipifconnectfailure.inc';
require __DIR__.'/connect.inc';
if (strcasecmp($user, "system") && strcasecmp($user, "sys"))
    die("skip needs to be run as a DBA user");
if ($test_drcp)
    die("skip as Output might vary with DRCP");
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

error_reporting(E_ALL ^ E_DEPRECATED);

/* In 11.2, there can only be one child edition.  So this test will
 * fail to create the necessary editions if a child edition exists
 * already
 */

$testuser     = 'testuser_ed_2';  // Used in conn_attr.inc
$testpassword = 'testuser';

require __DIR__."/conn_attr.inc";

echo"**Test 1.1 - Default value for  the attribute **************\n";
get_edit_attr($c);

echo"\n\n**Test 1.2 - Set a value and get the same with different connections *********\n";
set_edit_attr('MYEDITION');

// With oci_connect, oci_pconnect, oci_new_connect
$conn1 = get_conn(1);
get_edit_attr($conn1);

//pconnect
$conn2 = get_conn(2);
get_edit_attr($conn2);

//new_connect
$conn3 = get_conn(3);
get_edit_attr($conn3);

oci_close($conn1);

// With a oci_pconnect with a different charset.
$pc1 = oci_pconnect($testuser,$testpassword,$dbase,"utf8");
get_edit_attr($pc1);
oci_close($pc1);


echo"\n\n**Test 1.3 change the value and verify with existing connections.*********\n";
set_edit_attr('MYEDITION1');
get_edit_attr($conn2);
get_edit_attr($conn3); // Old value
oci_close($conn2);
oci_close($conn3);

//open a new connection and get the edition value . This will have the updated value.
$c3 = get_conn(3); //oci_new_connect()
get_edit_attr($c3);

$c4 = get_conn(2); //oci_pconnect()
get_edit_attr($c4);

$c5 = get_conn(1); //oci_connect()
get_edit_attr($c5);

oci_close($c3);
oci_close($c4);
oci_close($c5);

echo "\n\n**Test 1.4 - with different type of values *********\n";
$values_array = array(123,NULL,'NO EDITION','edition name which has more than thirty chars!!!edition name which has more than thirty chars!!!');
foreach ($values_array as $val ) {
    set_edit_attr($val);
    $c1 = get_conn(1); //oci_connect()
    if ($c1) {
        get_edit_attr($c1);
        oci_close($c1);
    }
}

echo "\n\n**Test 1.5 - Negative case with an invalid string value. *********\n";
$c1 = get_conn(3);
$r = set_edit_attr($c1);

echo"\n\n**Test 1.6 - Set Multiple times.*****\n";
set_edit_attr('MYEDITION');
set_edit_attr('MYEDITION1');
$c1 = get_conn(1);
get_edit_attr($c1);
oci_close($c1);

echo "\n\n**Test 1.7 - Test with ALTER SESSION statement to change the edition *******\n";
// Set the edition value to MYEDITION. open a conn .get the value.
// execute the alter system set edition ='MYEDITION' .get the value .
// set it back to MYEDITION using oci_set_edition. and get the value.

set_edit_attr('MYEDITION');
$c1 = get_conn(3);
echo "get the value set to MYEDITION with oci_set_edition\n";
get_edit_attr($c1);

$alter_stmt = "alter session set edition = MYEDITION1";
$s = oci_parse($c1,$alter_stmt);
oci_execute($s);
oci_commit($c1);
echo "Get the value set to MYEDITION1 with alter session\n";
get_edit_attr($c1);

echo " Get the value with a new connection\n";
$c2 = get_conn(1);
get_edit_attr($c2);

echo " Set the value back using oci-set_edition\n";
set_edit_attr('MYEDITION');
get_edit_attr($c2);

echo " Get the value with a new connection\n";
$c3 = get_conn(1);
get_edit_attr($c3);

oci_close($c1);
oci_close($c2);
oci_close($c3);


echo "\n\n**Test 1.8 - Test setting the attribute with scope ends*******\n";
set_scope();
get_scope();

clean_up($c);
echo "Done\n";


function set_scope() {
    $r = set_edit_attr('MYEDITION1');
}

function get_scope() {
    $sc1 = oci_connect($GLOBALS['testuser'],$GLOBALS['testpassword'],$GLOBALS['dbase']);
    if ($sc1 === false) {
        $m = oci_error();
        die("Error:" . $m['message']);
    }
    get_edit_attr($sc1);
    oci_close($sc1);
}
?>
--EXPECTF--
**Test 1.1 - Default value for  the attribute **************
The value of current EDITION is ORA$BASE


**Test 1.2 - Set a value and get the same with different connections *********
 The value of edition has been successfully set
Testing with oci_connect()
The value of current EDITION is MYEDITION
Testing with oci_pconnect()
The value of current EDITION is MYEDITION
Testing with oci_new_connect()
The value of current EDITION is MYEDITION
The value of current EDITION is MYEDITION


**Test 1.3 change the value and verify with existing connections.*********
 The value of edition has been successfully set
The value of current EDITION is MYEDITION
The value of current EDITION is MYEDITION
Testing with oci_new_connect()
The value of current EDITION is MYEDITION1
Testing with oci_pconnect()
The value of current EDITION is MYEDITION1
Testing with oci_connect()
The value of current EDITION is MYEDITION1


**Test 1.4 - with different type of values *********
 The value of edition has been successfully set
Testing with oci_connect()

Warning: oci_connect(): ORA-38801: %s ORA_EDITION in %s on line %d
 The value of edition has been successfully set
Testing with oci_connect()
The value of current EDITION is ORA$BASE
 The value of edition has been successfully set
Testing with oci_connect()

Warning: oci_connect(): ORA-38801: %s ORA_EDITION in %s on line %d
 The value of edition has been successfully set
Testing with oci_connect()

Warning: oci_connect(): ORA-38801: %s ORA_EDITION in %s on line %d


**Test 1.5 - Negative case with an invalid string value. *********
Testing with oci_new_connect()

Warning: oci_new_connect(): ORA-38801: %s ORA_EDITION in %s on line %d
 The value of edition has been successfully set


**Test 1.6 - Set Multiple times.*****
 The value of edition has been successfully set
 The value of edition has been successfully set
Testing with oci_connect()
The value of current EDITION is MYEDITION1


**Test 1.7 - Test with ALTER SESSION statement to change the edition *******
 The value of edition has been successfully set
Testing with oci_new_connect()
get the value set to MYEDITION with oci_set_edition
The value of current EDITION is MYEDITION
Get the value set to MYEDITION1 with alter session
The value of current EDITION is MYEDITION1
 Get the value with a new connection
Testing with oci_connect()
The value of current EDITION is MYEDITION
 Set the value back using oci-set_edition
 The value of edition has been successfully set
The value of current EDITION is MYEDITION
 Get the value with a new connection
Testing with oci_connect()
The value of current EDITION is MYEDITION


**Test 1.8 - Test setting the attribute with scope ends*******
 The value of edition has been successfully set
The value of current EDITION is MYEDITION1
Done

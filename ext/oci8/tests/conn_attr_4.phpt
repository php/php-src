--TEST--
Set and get of connection attributes with errors.
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');

if (strcasecmp($user, "system") && strcasecmp($user, "sys")) die("skip needs to be run as a DBA user");
if ($test_drcp) die("skip output might vary with DRCP");

preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) &&
      (($matches[1] == 11 && $matches[2] >= 2) ||
       ($matches[1] >= 12)
       ))) {
    // Bug fixed in 11.2 prevents client_info being reset
	die("skip expected output only valid when using Oracle 11gR2 or greater database server");
}
?>
--FILE--
<?php

$testuser     = 'testuser_attr_4';  // Used in conn_attr.inc
$testpassword = 'testuser';

require(__DIR__."/conn_attr.inc");

$attr_array = array('MODULE','ACTION','CLIENT_INFO','CLIENT_IDENTIFIER');

echo"**Test  Negative cases************\n";

echo "\nInvalid Connection resource\n";
$nc1=NULL;
// Invalid connection handle.
var_dump(oci_set_action($nc1,$nc1));

// Variable instead of a connection resource.
echo "\nInvalid Connection resource 2\n";
$str1= 'not a conn';
var_dump(oci_set_client_info($str1,$str1));

// Setting an Invalid value.
echo "\nInvalid Value \n";
$c1=oci_connect($testuser,$testpassword,$dbase);
var_dump(oci_set_action($c1,$c1));

// Setting values multiple times.
echo "\nSet Values multiple times \n";
var_dump(oci_set_action($c1,'ACTION1'));
var_dump(oci_set_action($c1,'ACTION1'));
var_dump(oci_set_action($c1,'ACTION2'));
var_dump(oci_set_action($c1,'ACTION1'));
get_attr($c1,'ACTION');

// Testing with different types of values
// NB.  This may diff in 11.1.0.6 due to a bug causing CLIENT_INFO of NULL to be ignored.
echo "\nSetting to different values \n";
$values_array = array(1000,NULL,'this is a very huge string with a length  > 64 !!!!!this is a very huge string with a length  > 64 !!!!!this is a very huge string with a length  > 64 !!!!!this is a very huge string with a length  > 64 !!!!!');

foreach($values_array as $val ) {
	oci_set_module_name($c1,$val);
	oci_set_client_identifier($c1,$val);
	oci_set_client_info($c1,$val);
	$r = oci_set_action($c1,$val);
	if ($r) {
		echo "Values set successfully to $val\n";
		foreach($attr_array as $attr) {
            get_attr($c1,$attr);
        }
	}
}

clean_up($c);
echo "Done\n";
?>
--EXPECTF--
**Test  Negative cases************

Invalid Connection resource

Warning: oci_set_action() expects parameter 1 to be resource, null given in %s on line %d
NULL

Invalid Connection resource 2

Warning: oci_set_client_info() expects parameter 1 to be resource, %s given in %s on line %d
NULL

Invalid Value 

Warning: oci_set_action() expects parameter 2 to be %s, resource given in %s on line %d
NULL

Set Values multiple times 
bool(true)
bool(true)
bool(true)
bool(true)
The value of ACTION is ACTION1

Setting to different values 
Values set successfully to 1000
The value of MODULE is 1000
The value of ACTION is 1000
The value of CLIENT_INFO is 1000
The value of CLIENT_IDENTIFIER is 1000
Values set successfully to 
The value of MODULE is 
The value of ACTION is 
The value of CLIENT_INFO is 
The value of CLIENT_IDENTIFIER is 

Warning: oci_set_module_name(): ORA-24960: %s OCI_ATTR_MODULE %s on line %d

Warning: oci_set_client_identifier(): ORA-24960: %s OCI_ATTR_CLIENT_IDENTIFIER %s on line %d

Warning: oci_set_client_info(): ORA-24960: %s OCI_ATTR_CLIENT_INFO %s on line %d

Warning: oci_set_action(): ORA-24960: %s OCI_ATTR_ACTION %s on line %d
Done

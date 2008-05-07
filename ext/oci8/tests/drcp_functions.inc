<?php

/* This file contains functions required by the DRCP tests */

function drcp_create_table($conn)
{
	$create_sql = "CREATE TABLE DRCPTEST (id NUMBER, name VARCHAR2(10), dept VARCHAR2(10))";
	$statement = oci_parse($conn, $create_sql);
	oci_execute($statement);
	
	$id_values = array(100,101,102,103,104,105,106,107,108);
	$name_values = array("WIILIAMS","JOHN","SMITH","JONES","ADAMS","ROBERT",
						 "BILL","LAWSON","MARY");
	$dept_values = array("ACCOUNTS","HR","HR","ADMIN","ACCOUNTS","HR",
						 "ACCOUNTS","HR","ACCOUNTS");
	for($i=0; $i<8; $i++) {
		$insert = "INSERT INTO DRCPTEST VALUES('".$id_values[$i]."','". $name_values[$i]."','".$dept_values[$i]."')";
		$s = oci_parse($conn, $insert);
		oci_execute($s);
	}	
}

function drcp_drop_table($conn)
{
	$ora_sql = "DROP TABLE DRCPTEST";
	$statement = oci_parse($conn, $ora_sql);
	oci_execute($statement);
}

function drcp_update_table($conn)
{
	$update_stmt ="Update drcptest set dept ='NEWDEPT' where id = 105";
	$s1 = oci_parse($conn,$update_stmt);
	oci_execute($s1,OCI_DEFAULT);
	echo "Update done-- DEPT value has been set to NEWDEPT\n";
}

function drcp_select_value($conn)
{
	$sel_stmt="select dept from drcptest where id=105";
	$s2 = oci_parse($conn,$sel_stmt);
	oci_execute($s2,OCI_DEFAULT);
	while(oci_fetch($s2)) {
		echo "The value of DEPT for id 105 is ".oci_result($s2,1)."\n";
	}
}

function drcp_select_packagevar($conn)
{
	$sel_stmt="select drcp_test_package.f1 as f1 from dual";
	$s2 = oci_parse($conn, $sel_stmt);
	oci_define_by_name($s2,'f1',$ret_num);
	oci_execute($s2);
	while(oci_fetch($s2)) {
		echo " The value of the package variable is ".oci_result($s2,1)."\n";
	}
}


function drcp_set_packagevar($conn,$num)
{
	$set_stmt = "begin drcp_test_package.p1($num); end;";
	$s1 = oci_parse($conn,$set_stmt);
	oci_execute($s1);
	echo " Package variable value set to " .$num."\n";
}

function drcp_create_package($c)
{
	$create_package_stmt = "create or replace package drcp_test_package as
			var int :=0;
	 		procedure p1(var1 int);
			function f1 return number;
			end;";
	$s1 = oci_parse($c, $create_package_stmt);
	oci_execute($s1);
	
	$package_body = "create or replace package body drcp_test_package as
		procedure p1(var1 int) is
		begin
		var :=var1;
		end;	
		function f1 return number is
		begin
		return drcp_test_package.var;
		end;
		end;";

	$s2 = oci_parse($c, $package_body);
	oci_execute($s2);
}

?>

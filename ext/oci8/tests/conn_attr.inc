<?php

require(dirname(__FILE__)."/connect.inc");

preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if ((isset($matches[1]) && $matches[1] >= 11)) {
    // Server is Oracle 11.2+
    $stmtarray = array(
        "drop user $testuser cascade",
        "create user $testuser identified by $testpassword",  // $testuser should be set by the file that includes conn_attr.inc
        "grant connect,resource,dba to $testuser",
        "alter user $testuser enable editions",
        "drop edition myedition1 cascade",
        "drop edition myedition cascade",
        "grant create any edition to $testuser",
        "create edition myedition",
        "create edition myedition1 as child of myedition",
        "grant use on edition myedition to $testuser",
        "grant use on edition myedition1 to $testuser",
    );
} else {
    // Server is Pre 11.2
    $stmtarray = array(
        "drop user $testuser cascade",
        "create user $testuser identified by $testpassword",
        "grant connect,resource,dba to $testuser",
    );
}

foreach ($stmtarray as $stmt) {
    $s = oci_parse($c, $stmt);
    $r = @oci_execute($s);
	if (!$r) {
		$m = oci_error($s);
		if (!in_array($m['code'], array(   // ignore expected errors
                        942 // table or view does not exist
                     , 1918 // user does not exist
                     , 2289 // sequence does not exist
                     , 4080 // trigger does not exist
                    , 38802 // edition does not exist
                ))) {
			echo "Error:" . $stmt . PHP_EOL . $m['message'] . PHP_EOL;
            if ($m['code'] == 38807) {
                echo "You appear to already have an edition in use that prevents this PHP test from running.  Query DBA_EDITIONS to see existing editions.". PHP_EOL;
            }
            die;
		}
	}
}

function get_attr($conn,$attr)
{
    $sel_stmt="select " .$attr. " from v\$session where sid =
	(select sid from v\$session where audsid =
	sys_context('userenv','sessionid')) order by 1";
    $s2 = oci_parse($conn,$sel_stmt);
    oci_execute($s2,OCI_DEFAULT);
    while (oci_fetch($s2)) {
        echo "The value of ".$attr ." is ".oci_result($s2,1)."\n";
    }
}

/* Pass $conn_type=1 for a connection with oci_connect()
   Pass $conn_type=2 for ooci_pconnect
   Default will give a oci_new_connect */

function get_conn($conn_type)
{
	$user = $GLOBALS['testuser'];
	$password = $GLOBALS['testpassword'];
	$dbase = $GLOBALS['dbase'];
	switch($conn_type) {
        case 1:
            echo "Testing with oci_connect()\n";
            return(oci_connect($user,$password,$dbase));
            break;
        case 2:
            echo "Testing with oci_pconnect()\n";
            return(oci_pconnect($user,$password,$dbase));
            break;
        default:
            echo "Testing with oci_new_connect()\n";
            return(oci_new_connect($user,$password,$dbase));
            break;
	}
}

function set_attr($conn,$attr,$sufix)
{
	if (!strcmp($attr,'MODULE'))
		$r = oci_set_module_name($conn,'PHP TEST'.$sufix);
	else if (!strcmp($attr,'ACTION'))
        $r = oci_set_action($conn,'TASK'.$sufix);
	else if (!strcmp($attr,'CLIENT_INFO'))
        $r = oci_set_client_info($conn,'INFO1'.$sufix);
	else if (!strcmp($attr,'CLIENT_IDENTIFIER'))
        $r = oci_set_client_identifier($conn,'ID00'.$sufix);
	else
		echo "Pass one of the above four attributes!!!\n";
	if ($r) {
		echo "Value of $attr has been set successfully\n";
    }

	//Do a round-trip here
	oci_server_version($conn);
	return $r;
}

function set_edit_attr($value)
{
	$r = oci_set_edition($value);
    if ($r) {
		echo " The value of edition has been successfully set\n";
    }
	return $r;
}

function get_edit_attr ($conn) {
	$sel_stmt = "select sys_context('USERENV', 'CURRENT_EDITION_NAME') from dual";
    $s2 = oci_parse($conn,$sel_stmt);
    oci_execute($s2,OCI_DEFAULT);
    while (oci_fetch($s2)) {
        echo "The value of current EDITION is ".oci_result($s2,1)."\n";
    }
}

function get_sys_attr($conn,$attr)
{
	$sel_stmt="select " .$attr. " from v\$session where sid =
	(select sid from v\$session where audsid = sys_context('userenv','sessionid')) order by 1";
    $s2 = oci_parse($conn,$sel_stmt);
    oci_execute($s2,OCI_DEFAULT);
    while (oci_fetch($s2)) {
        echo "The value of ".$attr ." is ".oci_result($s2,1)."\n";
    }
}

function clean_up($c) {
	$stmtarray = array(
        "drop edition myedition1 cascade",
        "drop edition myedition cascade",
        "drop user " . $GLOBALS['testuser'] . " cascade",
	);

	foreach ($stmtarray as $stmt) {
        $s = oci_parse($c, $stmt);
        @oci_execute($s);
	}
}

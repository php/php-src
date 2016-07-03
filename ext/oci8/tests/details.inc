<?php

/*
 * Please change $user, $password and $dbase to match your configuration.
 *
 * Set $test_drcp to TRUE if you want to run the Oracle Database
 * Resident Connection Pooling (DRCP) tests. For these tests to run
 * successfully, you need a server and client which is Oracle 11g or
 * greater, and $dbase should be set to the tnsnames.ora entry
 * corresponding to the POOLED server instance or an Easy Connect
 * string like hostname:port/service_name:POOLED
 */

if (file_exists(dirname(__FILE__)."/details_local.inc")) {
	include(dirname(__FILE__)."/details_local.inc");   // this file is not part of the source distribution; make it your own local variant of details.inc
} else {
	if (false !== getenv('PHP_OCI8_TEST_DB')) {
		$user		= getenv('PHP_OCI8_TEST_USER');	  // Database username for tests
		$password	= getenv('PHP_OCI8_TEST_PASS');	  // Password for $user
		$dbase		= getenv('PHP_OCI8_TEST_DB');	  // Database connection string
		$test_drcp	= getenv('PHP_OCI8_TEST_DRCP');
		if (false !== $test_drcp && 0 == strcasecmp($test_drcp,'TRUE')) {
			$test_drcp = TRUE;
		} else {
			$test_drcp = FALSE;
		}
	} else {
		$user						= "system";
		$password					= "oracle";
		$dbase						= "localhost/XE";
		$test_drcp					= FALSE;
	}
	
	/*
	 * Common object names for scripts to use
	 */
	
	$table_name = "tb".substr(str_replace(Array(".", "-"), "_", php_uname("n")), 0, 5);
	$type_name = strtoupper("tp".substr(str_replace(Array(".", "-"), "_", php_uname("n")), 0, 5));
	$schema = '';
}


/*
 * Used for creating/dropping schema objects used by a test
 */

if (!function_exists('oci8_test_sql_execute')) {
    function oci8_test_sql_execute($c, $stmtarray)
    {
        foreach ($stmtarray as $stmt) {
            $s = oci_parse($c, $stmt);
            if (!$s) {
                $m = oci_error($c);
                echo "oci8_test_sql_execute() error:". PHP_EOL . $stmt . PHP_EOL . $m['message'] . PHP_EOL;
            }
            else {
                $r = @oci_execute($s);
                if (!$r) {
                    $m = oci_error($s);
                    if (!in_array($m['code'], array(   // ignore expected errors
								    942 // table or view does not exist
                                ,  1918 // user does not exist
                                ,  2024 // database link not found
                                ,  2289 // sequence does not exist
                                ,  4080 // trigger does not exist
                                , 38802 // edition does not exist
                            ))) {
			echo "oci8_test_sql_execute() error:". PHP_EOL . $stmt . PHP_EOL . $m['message'] . PHP_EOL;
                    }
                }
            }
        }
    }

}

?>

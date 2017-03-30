<?php

// Create connection
$conn = oci_pconnect('username', 'password', 'pool');
if (!$conn) {
    $e = oci_error();
    trigger_error($e['message'], E_USER_ERROR);
}

// Register TAF callback
oci_register_taf_callback($conn, 'tafCallback');

// Do something, that requires TAF callbacks
switchSchema($conn);

// Do some stuff

// For testing:
//   Do a long SELECT statement and sleep between
//   each fetch. Start the script and while it's
//   running shutdown the server it is connected to.
//   This way a failover is triggered and the
//   callback function is being called.

// Close connection
oci_close($conn);




/* Switches the schema on database
 *
 * Without TAF callback, the script would fail
 * once a failover occurs, as the new server
 * still has the default schema. Using TAF
 * callback the necessary settings can be setup
 * for the new server.
 */
function switchSchema($conn) {
	$stid = oci_parse($conn, 'ALTER SESSION SET CURRENT_SCHEMA = foobar');
	oci_execute($stid);
	oci_free_statement($stid);
}

/* Userspace callback function
 *
 * This function is a modified version of
 * cdemofo.c from the following package:
 * "Oracle Database Examples (12.1.0.2.0)
 *  for Microsoft Windows (x64)"
 */
function tafCallback($conn, $event, $type) {
	switch ($event) {
		case OCI_FO_BEGIN:
			printf(" Failing Over ... Please stand by \n");
			printf(" Failover type was found to be %s \n",
					(($type==OCI_FO_NONE) ? "NONE"
					:($type==OCI_FO_SESSION) ? "SESSION"
					:($type==OCI_FO_SELECT) ? "SELECT"
					: "UNKNOWN!"));
			break;

		case OCI_FO_ABORT:
			printf(" Failover aborted. Failover will not take place.\n");
			break;

		case OCI_FO_END:
			printf(" Failover ended ...switching schema");
			switchSchema($conn);
			printf(" ...resuming services\n");
			break;

		case OCI_FO_REAUTH:
			printf(" Failed over user. Switching schema.");
			switchSchema($conn);
			printf(" Resuming services\n");
			break;

		case OCI_FO_ERROR:
			printf(" Failover error gotten. Sleeping...\n");
			sleep(3);
			return OCI_FO_RETRY;
			break;

		default:
			printf("Bad Failover Event: %d.\n", $event);
			break;
	}
}

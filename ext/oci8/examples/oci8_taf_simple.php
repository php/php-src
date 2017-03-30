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

/* Userspace callback function */
function tafCallback($conn, $event) {
	// At the end of a successfull failover, the event
    // will either be OCI_FO_END or OCI_FO_REAUTH
	if($event == OCI_FO_END || $event == OCI_FO_REAUTH) {
		// Do something, that requires TAF callbacks
		switchSchema($conn);
	}
}

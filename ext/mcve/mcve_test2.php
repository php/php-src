<?
 #
 # $Id$
 #

$connect_type = "IP";

dl("php_mcve.so");

$conn = MCVE_InitConn();
print "MCVE_InitConn() returned $conn<br>\n";

if ($connect_type == "IP") {
	MCVE_SetIP($conn, "localhost", 8333) or
		die("MCVE_SetIP() failed");
	print "MCVE_SetIP() successful<br>\n";
} else {
	MCVE_SetDropFile($conn, "/var/mcve/trans") or
		die("MCVE_SetDropFile() failed");
	print "MCVE_SetDropFile() successful<br>\n";
}

MCVE_Connect($conn) or
	die("MCVE_Connect() failed");
print "MCVE_Connect() successful<br>\n";

# send a request
$ident = MCVE_Sale($conn, "test", "test", NULL, "5454545454545454",
    "1205", 11.00, NULL, NULL, NULL, NULL, "me", NULL, 54321);
if ($ident == -1)
	die("MCVE_Sale() failed");
else
	print "Identifier: $ident<br>\n";

$ident = MCVE_Sale($conn, "test", "test", NULL, "5454545454545454",
    "1205", 12.00, NULL, NULL, NULL, NULL, "me", NULL, 54321);
if ($ident == -1)
	die("MCVE_Sale() failed");
else
	print "Identifier: $ident<br>\n";

$pending = 0;
$complete = -1;
while ($pending != $complete) {
	sleep(2);

	MCVE_Monitor($conn);

	$pending = MCVE_TransInQueue($conn);
	print "Transactions pending: $pending<br>\n";

	$complete = MCVE_CompleteAuthorizations($conn, &$list);
	print "Authorizations complete: $complete<br>\n";

	flush();
}

for ($i = 0; $i < $complete; $i++) {
	$status = MCVE_CheckStatus($conn, $i);
	print "Transaction #" . $list[$i] . " complete: $status<br>\n";
}

MCVE_DestroyConn($conn);
print "MCVE_DestroyConn() completed<br>\n";

#phpinfo();

?>

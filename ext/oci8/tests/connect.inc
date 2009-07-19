<?php

if (file_exists("details_local.inc")) {
	include("details_local.inc"); // this file is not part of the source distribution; make it your own local variant of details.inc
} else {
	include "details.inc";
}

if (!empty($dbase)) {
	$c = @oci_connect($user, $password, $dbase);
}
else {
	$c = @oci_connect($user, $password);
}

if (!$c) {
	echo "connect.inc: Failed to connect as '$user' to '$dbase'\n";
	$e = oci_error();
	echo $e['message']."\n";
}

?>

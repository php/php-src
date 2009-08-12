<?php

require("details.inc");

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

<?php

require 'details.inc';

if (!empty($dbase)) {
    $c = @oci_connect($user, $password, $dbase);
}
else {
    $c = @oci_connect($user, $password);
}

if (!$c) {
    $m = oci_error();
    trigger_error("connect.inc: Failed to connect as '$user' to '$dbase': ". $m['message'], E_USER_ERROR);
}

?>

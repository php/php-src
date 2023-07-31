<?php

require "details.inc";

// the oci_connect parameter corresponding to $dbase defaults to null
if (!isset($dbase)) {
    $dbase = null;
}

$c = @oci_connect($user, $password, $dbase);

if ($c) {
    oci_close($c);
}
else {
    $msg = "skip Failed to connect to Oracle Database instance ";
    if (is_null($dbase)) {
        $msg .= "<default>";
    }
    else {
        $msg .= "\"$dbase\"";
    }
    $msg .= " as user \"$user\"";
    $e = oci_error();
    $msg .= ": {$e['message']}";
    die($msg);
}

?>

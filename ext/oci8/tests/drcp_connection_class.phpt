--TEST--
DRCP: oci8.connection_class with ini_get() and ini_set()
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die("skip no oci8 extension");
preg_match('/^[[:digit:]]+/', oci_client_version(), $matches);
if (!(isset($matches[0]) && $matches[0] >= 11)) {
    die("skip works only with Oracle 11g or greater version of Oracle client libraries");
}
?>
--INI--
oci8.connection_class=test
--FILE--
<?php

echo "Setting a new connection class now\n";
ini_set('oci8.connection_class',"New cc");

// Get the New connection class name. Should return New CC

$new_cc = ini_get('oci8.connection_class');
echo "The New oci8.connection_class is $new_cc\n";

echo "Done\n";

?>
--EXPECT--
Setting a new connection class now
The New oci8.connection_class is New cc
Done

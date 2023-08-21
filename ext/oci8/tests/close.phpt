--TEST--
connect/close/connect
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

oci_close($c);

oci_connect($user, $password, $dbase);

echo "Done\n";
?>
--EXPECT--
Done

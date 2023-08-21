--TEST--
Exercise OCIPing functionality on reconnect (code coverage test)
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--INI--
oci8.ping_interval=0
--FILE--
<?php

require __DIR__.'/details.inc';

for ($i = 0; $i < 2; $i++) {
    if (!empty($dbase)) {
        $c = oci_pconnect($user,$password,$dbase);
    }
    else {
        $c = oci_pconnect($user,$password);
    }
}

echo "Done\n";

?>
--EXPECT--
Done

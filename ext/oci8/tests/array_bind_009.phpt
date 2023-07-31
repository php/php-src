--TEST--
oci_bind_array_by_name() and invalid values 9
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

var_dump(oci_bind_array_by_name($c, ":c1", $array, 5, 5, SQLT_CHR));
?>
--EXPECTF--
Fatal error: Uncaught TypeError: oci_bind_array_by_name(): supplied resource is not a valid oci8 statement resource in %s:%d
Stack trace:
#0 %s: oci_bind_array_by_name(%s)
#1 {main}
  thrown in %s on line %d
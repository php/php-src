--TEST--
oci8.old_oci_close_semantics On
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--INI--
oci8.old_oci_close_semantics=1
--FILE--
<?php

require __DIR__."/connect.inc";

var_dump($c);
var_dump(oci_close($c));
var_dump(oci_parse($c, "select 1 from dual"));

echo "Done\n";

?>
--EXPECTF--
Deprecated: Directive oci8.old_oci_close_semantics is deprecated%s
resource(%d) of type (oci8 connection)
NULL
resource(%d) of type (oci8 statement)
Done

--TEST--
64-bit bigendian platform, PL/SQL, assign to bind-variable, string type
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
require(__DIR__.'/connect.inc');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Run Test

echo "64-bit bigendian platform, PL/SQL, assign to bind-variable, string type\n";

$sql = "begin
          :output1 := 'Teststr';
        end;";

$s = oci_parse($c, $sql);
oci_bind_by_name($s, ':output1', $output1, 64);
oci_execute($s);
print $output1."\n"
?>
--EXPECT--
64-bit bigendian platform, PL/SQL, assign to bind-variable, string type
Teststr

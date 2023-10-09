<?php

// Check the test file is valid for this DB type
require dirname(__FILE__).'/connect.inc';
$v = oci_server_version($c);
if (strpos($v, 'Oracle Database') !== false)
    $dbtype = 'oracledb';
else if (strpos($v, 'Oracle TimesTen') !== false)
    $dbtype = 'timesten';
else {
    die("skip Unknown database type $v\n");
}
if (!array_key_exists($dbtype, $target_dbs) || $target_dbs[$dbtype] !== true)
    die("skip Test not valid for $v\n");

?>

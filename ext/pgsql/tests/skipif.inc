<?php
// This script prints "skip" unless:
// * the pgsql extension is built-in or loadable, AND
// * there is a database called "test" accessible
//   with no username/password, AND
// * we have create/drop privileges on the entire "test"
//   database

include("config.inc");

if (!extension_loaded("pgsql")) {
    die("skip\n");
}
$conn = @pg_connect($conn_str);
if (!is_resource($conn)) {
    die("skip could not connect\n");
}
?>
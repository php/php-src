<?php
// This script prints "skip" unless:
// * the pgsql extension is built-in or loadable, AND
// * there is a database called "test" accessible
//   with no username/password, AND
// * we have create/drop privileges on the entire "test"
//   database

include("config.inc");
include("lcmess.inc");

if (!extension_loaded("pgsql")) {
    die("skip pgsql extension not loaded\n");
}
$conn = @pg_connect($conn_str);
if (!is_resource($conn)) {
    die("skip could not connect\n");
}

function skip_server_version($version, $op = '<')
{
    $pg = pg_parameter_status('server_version');
    if (version_compare($pg, $version, $op)) {
        die("skip Server version {$pg} is {$op} {$version}\n");
    }
    return $pg;
}

function skip_bytea_not_hex()
{
    $out = pg_escape_bytea("\xFF");
    if (strpos($out, '377') !== false) {
        die("skip libpq or backend < 9.0\n");
    }
}

function skip_bytea_not_escape()
{
    $out = pg_escape_bytea("\xFF");
    if (strpos($out, '377') === false) {
        die("skip libpq or backend >= 9.0\n");
    }
}

?>

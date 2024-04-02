
<?php
// This script prints "skip" unless:
// * the pgsql extension is built-in or loadable, AND
// * there is a database called "test" accessible
//   with no username/password, AND
// * we have create/drop privileges on the entire "test"
//   database

include("config.inc");
include("lcmess.inc");

if (getenv("SKIP_REPEAT")) {
    // pgsql tests are order-dependent.
    // We should probably change that, but in the meantime do not allow repetition.
    die("skip Cannot repeat pgsql tests");
}
$conn = @pg_connect($conn_str);
if (!$conn) {
    die("skip could not connect\n");
}

function skip_server_version($version, $op = '<')
{
    global $conn;
    $pg = pg_parameter_status($conn,'server_version');
    if (version_compare($pg, $version, $op)) {
        die("skip Server version {$pg} is {$op} {$version}\n");
    }
    return $pg;
}

function skip_bytea_not_hex()
{
    global $conn;
    $out = pg_escape_bytea($conn, "\xFF");
    if (strpos($out, '377') !== false) {
        die("skip libpq or backend < 9.0\n");
    }
}

function skip_bytea_not_escape()
{
    global $conn;
    $out = pg_escape_bytea($conn, "\xFF");
    if (strpos($out, '377') === false) {
        die("skip libpq or backend >= 9.0\n");
    }
}

?>

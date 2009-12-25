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

function skip_server_version($version, $op = '<') { _skip_version('server', $version, $op); }
function skip_client_version($version, $op = '<') { _skip_version('client', $version, $op); }


function _skip_version($type, $version, $op)
{
	$pg = pg_parameter_status($type.'_version');
	if (version_compare($pg, $version, $op)) {
		die("skip {$type} version {$pg} is {$op} {$version}\n");
	}
}

?>

<?php
// This script prints "skip" unless:
// * the pgsql extension is built-in or loadable, AND
// * there is a database called "test" accessible
//   with no username/password, AND
// * we have create/drop privileges on the entire "test"
//   database
if (!extension_loaded("pgsql") && ini_get("enable_dl")) {
  $dlext = (substr(PHP_OS, 0, 3) == "WIN") ? ".dll" : ".so";
  @dl("pgsql$dlext");
}
if (!extension_loaded("pgsql")) {
    die("skip\n");
}
$conn = @pg_connect("host=localhost dbname=test");
if (!is_resource($conn)) {
    die("skip\n");
}
?>
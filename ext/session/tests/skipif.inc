<?php
// This script prints "skip" if condition does not meet.
if (!extension_loaded("session") && ini_get("enable_dl")) {
  $dlext = (substr(PHP_OS, 0, 3) == "WIN") ? ".dll" : ".so";
  @dl("session$dlext");
}
if (!extension_loaded("session")) {
    die("skip\n");
}
?>

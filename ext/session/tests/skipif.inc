<?php
// This script prints "skip" if condition does not meet.
if (!extension_loaded("session") && ini_get("enable_dl")) {
  $dlext = (substr(PHP_OS, 0, 3) == "WIN") ? ".dll" : ".so";
  @dl("session$dlext");
}
if (!extension_loaded("session")) {
    die("skip\n");
}
$save_path = ini_get("session.save_path");
if (!@is_writable($save_path)) {
	if (($p = strpos($save_path, ';')) !== false) {
		$save_path = substr($save_path, ++$p);
	}
	if (!@is_writable($save_path)) {
		die("skip\n");
	}
}
?>

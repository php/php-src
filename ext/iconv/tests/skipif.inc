<?php
// This script prints "skip" if condition does not meet.

// Do not dl load extension
//if (!extension_loaded("iconv") && ini_get("enable_dl")) {
//  $dlext = (substr(PHP_OS, 0, 3) == "WIN") ? ".dll" : ".so";
//  @dl("iconv$dlext");
//}
if (!function_exists("iconv")) {
    die("skip iconv function not available\n");
}
?>
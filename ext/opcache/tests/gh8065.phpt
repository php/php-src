--TEST--
GH-8065: opcache.consistency_checks > 0 causes segfaults in PHP >= 8.1.5 in fpm context
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.consistency_checks=1
opcache.log_verbosity_level=2
--FILE--
<?php
var_dump(ini_get("opcache.consistency_checks"));
var_dump(ini_set("opcache.consistency_checks", 1));
var_dump(ini_set("opcache.consistency_checks", -1));
var_dump(ini_set("opcache.consistency_checks", 0));
?>
--EXPECTF--
%sWarning opcache.consistency_checks is reset back to 0 because it does not work properly (see GH-8065, GH-10624).

string(1) "0"
%sWarning opcache.consistency_checks is reset back to 0 because it does not work properly (see GH-8065, GH-10624).

bool(false)
%sWarning opcache.consistency_checks is reset back to 0 because it does not work properly (see GH-8065, GH-10624).

bool(false)
string(1) "0"

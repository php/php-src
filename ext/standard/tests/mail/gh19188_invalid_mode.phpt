--TEST--
GH-19188: mail.cr_lf_mode runtime changes should fail
--FILE--
<?php

var_dump(ini_set('mail.cr_lf_mode', 'lf'));
var_dump(ini_get('mail.cr_lf_mode'));

var_dump(ini_set('mail.cr_lf_mode', 'mixed'));
var_dump(ini_get('mail.cr_lf_mode'));

var_dump(ini_set('mail.cr_lf_mode', 'os'));
var_dump(ini_get('mail.cr_lf_mode'));

var_dump(ini_set('mail.cr_lf_mode', 'invalid'));
var_dump(ini_get('mail.cr_lf_mode'));
?>
--EXPECT--
bool(false)
string(4) "crlf"
bool(false)
string(4) "crlf"
bool(false)
string(4) "crlf"
bool(false)
string(4) "crlf"

--TEST--
Bug #74514 5 session functions incorrectly warn when calling in read-only/getter mode.
--EXTENSIONS--
session
--SKIPIF--
<?php
include('skipif.inc');
?>
--FILE--
<?php
/*
CLI ignores HTTP headers at all, i.e. does not output any HTTP headers,
but it still uses SG(headers_sent).

CLI works as Web server, so SG(headers_sent) cannot be ignored nor changed.
Therefore, once HTTP header is considered as sent, these functions emits
'headers already sent' errors if they try to set new values.

Older PHPs(<7.2) did not care about this misuse on Web SAPI.
*/
var_dump(session_name('foo'));
var_dump(session_name());
var_dump(session_module_name());
var_dump(session_save_path());
var_dump(session_cache_limiter());
var_dump(session_cache_expire());
?>
--EXPECTF--
string(%d) "%S"
string(%d) "%S"
string(%d) "%S"
string(%d) "%S"
string(%d) "%S"
int(%d)

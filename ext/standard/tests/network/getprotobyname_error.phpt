--TEST--
getprotobyname function errors test
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--SKIPIF--
<?php
if (getenv('SKIP_MSAN')) die('skip msan missing interceptor for getprotobyname()');
?>
--FILE--
<?php
// invalid protocol name
var_dump(getprotobyname('abc'));
?>
--EXPECT--
bool(false)

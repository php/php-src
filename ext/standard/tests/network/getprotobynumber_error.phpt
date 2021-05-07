--TEST--
getprotobynumber function errors test
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--SKIPIF--
<?php
if (getenv('SKIP_MSAN')) die('skip msan missing interceptor for getprotobynumber()');
?>
--FILE--
<?php
// invalid protocol number
var_dump(getprotobynumber(999));
?>
--EXPECT--
bool(false)

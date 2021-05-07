--TEST--
getprotobyname function basic test
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--SKIPIF--
<?php
if(in_array(PHP_OS_FAMILY, ['BSD', 'Darwin', 'Solaris', 'Linux'])){
    if (!file_exists("/etc/protocols")) die("skip reason: missing /etc/protocols");
}
if (getenv('SKIP_MSAN')) die('skip msan missing interceptor for getprotobyname()');
?>
--FILE--
<?php
var_dump(getprotobyname('tcp'));
?>
--EXPECT--
int(6)

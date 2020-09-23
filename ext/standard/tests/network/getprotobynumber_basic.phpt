--TEST--
getprotobynumber function basic test
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--SKIPIF--
<?php
    if(in_array(PHP_OS_FAMILY, ['BSD', 'Darwin', 'Solaris', 'Linux'])){
        if (!file_exists("/etc/protocols")) die("skip reason: missing /etc/protocols");
    }
?>
--FILE--
<?php
	var_dump(getprotobynumber(6));
?>
--EXPECT--
string(3) "tcp"

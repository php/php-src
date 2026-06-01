--TEST--
mysqli_real_connect() retry on same handle does not corrupt mysqlnd connect_attr
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require 'connect.inc';

$link = mysqli_init();
@mysqli_real_connect($link, $host, $user, 'bogus_password_to_force_failure', $db, $port, $socket);

if (!mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
    printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
}

mysqli_close($link);

print "done!";
?>
--EXPECT--
done!

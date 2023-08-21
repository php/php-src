--TEST--
mysqli_get_server_info()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';
    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
        exit(1);
    }

    if (!is_string($info = mysqli_get_server_info($link)) || ('' === $info))
        printf("[003] Expecting string/any_non_empty, got %s/%s\n", gettype($info), $info);

    print "done!";
?>
--EXPECT--
done!

--TEST--
mysqli_get_server_version()
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

    /* 5.1.5 -> 50105 -- major_version*10000 + minor_version *100 + sub_version */
    /* < 30000 = pre 3.2.3, very unlikely! */
    if (!is_int($info = mysqli_get_server_version($link)) || ($info < (3 * 10000)))
        printf("[003] Expecting int/any >= 30000, got %s/%s\n", gettype($info), $info);

    print "done!";
?>
--EXPECT--
done!

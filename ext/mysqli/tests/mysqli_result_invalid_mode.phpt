--TEST--
mysqli_result(), invalid mode
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

    $valid = array(MYSQLI_STORE_RESULT, MYSQLI_USE_RESULT);
    $invalidModes = [-1, 152];
    foreach ($invalidModes as $mode) {
        try {
            new mysqli_result($link, $mode);
        } catch (\ValueError $e) {
            echo $e->getMessage() . \PHP_EOL;
        }
    }
?>
--EXPECT--
mysqli_result::__construct(): Argument #2 ($result_mode) must be either MYSQLI_STORE_RESULT or MYSQLI_USE_RESULT
mysqli_result::__construct(): Argument #2 ($result_mode) must be either MYSQLI_STORE_RESULT or MYSQLI_USE_RESULT

--TEST--
Deprecated messages for mysqli::get_client_info() method
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require 'connect.inc';

if (!$mysqli = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    printf("Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
        $host, $user, $db, $port, $socket);
    exit(1);
}

printf("client_info = '%s'\n", $mysqli->get_client_info());

printf("client_info = '%s'\n", mysqli_get_client_info($mysqli));

print "done!";
?>
--EXPECTF--

Deprecated: Method mysqli::get_client_info() is deprecated in %s
client_info = '%s'

Deprecated: mysqli_get_client_info(): Passing connection object as an argument is deprecated in %s
client_info = '%s'
done!

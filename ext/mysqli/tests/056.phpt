--TEST--
extend mysqli
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    class foobar extends mysqli {
        function test () {
            return ("I do not like MySQL 4.1");
        }
    }

    $foo = new foobar();
    $foo->connect($host, $user, $passwd, $db, $port, $socket);
    $foo->close();
    printf("%s\n", $foo->test());
?>
--EXPECT--
I do not like MySQL 4.1

--TEST--
Bug #54674 mysqlnd valid_sjis_(head|tail) is using invalid operator and range.
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--INI--
mysqli.max_links = 1
mysqli.allow_persistent = Off
mysqli.max_persistent = 0
--FILE--
<?php
    include 'connect.inc';

    $link = mysqli_init();
    if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
        printf("[002] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
    }

    $japanese_so = pack('H4', '835c');
    $link->set_charset('sjis');
    var_dump($link->real_escape_string($japanese_so) === $japanese_so);
    mysqli_close($link);

    print "done!";
?>
--EXPECT--
bool(true)
done!

--TEST--
function test: mysqli_num_fields() 2
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';

    /*** test mysqli_connect 127.0.0.1 ***/
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    mysqli_real_query($link, "SHOW VARIABLES");

    if (mysqli_field_count($link)) {
        $result = mysqli_store_result($link);
        $num = mysqli_num_fields($result);
        mysqli_free_result($result);
    }

    var_dump($num);

    mysqli_close($link);
    print "done!";
?>
--EXPECT--
int(2)
done!

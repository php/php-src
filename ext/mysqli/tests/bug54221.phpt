--TEST--
Bug #54221 mysqli::get_warnings segfault when used in multi queries
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--INI--
mysqli.max_links = 1
mysqli.allow_persistent = Off
mysqli.max_persistent = 0
mysqli.reconnect = Off
--FILE--
<?php
    include ("connect.inc");

    $link = mysqli_init();
    if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
        printf("[002] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
    }

    $create = "CREATE TEMPORARY TABLE IF NOT EXISTS t54221(a int)";

    $query = "$create;$create;$create;";
    if ($link->multi_query($query)) {
        do {
            $sth = $link->store_result();

            if ($link->warning_count) {
                $warnings = $link->get_warnings();
                if ($warnings) {
                    do {
                        echo "Warning: ".$warnings->errno.": ".$warnings->message."\n";
                    } while ($warnings->next());
                }
            }
        } while ($link->more_results() && $link->next_result());
    }

    mysqli_close($link);

    print "done!";
?>
--EXPECT--
Warning: 1050: Table 't54221' already exists
done!

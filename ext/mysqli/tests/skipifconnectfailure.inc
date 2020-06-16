<?php
require_once('connect.inc');
if ($skip_on_connect_failure) {
    include_once('connect.inc');
    $link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
    if (!is_object($link))
        die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));
    mysqli_close($link);
}
?>

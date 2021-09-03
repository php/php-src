--TEST--
Bug #55653 	PS crash with libmysql when binding same variable as param and out
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
    }

    $in_and_out = "a";

    if (!($stmt = $link->stmt_init()))
        printf("[002] [%d] %s\n", $link->errno, $link->error);

    if (!($stmt->prepare("SELECT ?")) ||
        !($stmt->bind_param("s", $in_and_out)) ||
        !($stmt->execute()) ||
        !($stmt->bind_result($in_and_out)))
        printf("[003] [%d] %s\n", $stmt->errno, $stmt->error);

    if (!$stmt->fetch())
        printf("[004] [%d] %s\n", $stmt->errno, $stmt->error);

    if ("a" !== $in_and_out)
        printf("[005] Wrong result: '%s'\n", $in_and_out);

    echo "done!";
?>
--EXPECT--
done!

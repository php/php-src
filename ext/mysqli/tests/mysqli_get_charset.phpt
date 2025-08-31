--TEST--
mysqli_get_charset()
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

    if (!$res = mysqli_query($link, 'SELECT @@character_set_connection AS charset, @@collation_connection AS collation'))
        printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    $tmp = mysqli_fetch_assoc($res);
    mysqli_free_result($res);
    if (!($character_set_connection = $tmp['charset']) || !($collation_connection = $tmp['collation']))
        printf("[008] Cannot determine current character set and collation\n");

    if (!$res = mysqli_query($link, $sql = sprintf("SHOW CHARACTER SET LIKE '%s'", $character_set_connection)))
        printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    if (!mysqli_fetch_assoc($res))
        printf("[010] Cannot fetch Maxlen and/or Comment, test will fail: $sql\n");

    if (!$res = mysqli_query($link, sprintf("SHOW COLLATION LIKE '%s'", $collation_connection)))
        printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    $tmp = mysqli_fetch_assoc($res);
    mysqli_free_result($res);
    if (!($id = $tmp['Id']))
        printf("[012] Cannot fetch Id/Number, test will fail\n");

    if (!$res = mysqli_query($link, sprintf("SHOW VARIABLES LIKE 'character_sets_dir'")))
        printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    $tmp = mysqli_fetch_assoc($res);
    mysqli_free_result($res);
    if (!($character_sets_dir = $tmp['Value']))
        printf("[014] Cannot fetch character_sets_dir, test will fail\n");

    if (!is_object($charset = mysqli_get_charset($link)))
        printf("[015] Expecting object/std_class, got %s/%s\n", gettype($charset), $charset);

    if (!isset($charset->charset) ||
        !is_string($charset->charset) ||
        $character_set_connection !== $charset->charset)
        printf("[016] Expecting string/%s, got %s/%s\n", $character_set_connection, gettype($charset->charset), $charset->charset);
    if (!isset($charset->collation) ||
        !is_string($charset->collation) ||
        $collation_connection !== $charset->collation)
        printf("[017] Expecting string/%s, got %s/%s\n", $collation_connection, gettype($charset->collation), $charset->collation);

    if (!isset($charset->dir) ||
        !is_string($charset->dir))
        printf("[019] Expecting string - ideally %s*, got %s/%s\n", $character_sets_dir, gettype($charset->dir), $charset->dir);

    if (!isset($charset->min_length) ||
        !(is_int($charset->min_length)) ||
        ($charset->min_length < 0) ||
        ($charset->min_length > $charset->max_length))
        printf("[020] Expecting int between 0 ... %d, got %s/%s\n", $charset->max_length,
            gettype($charset->min_length), $charset->min_length);

    if (!isset($charset->number) ||
        !is_int($charset->number) ||
        ($charset->number !== (int)$id))
        printf("[021] Expecting int/%d, got %s/%s\n", $id, gettype($charset->number), $charset->number);

    if (!isset($charset->state) ||
        !is_int($charset->state))
        printf("[022] Expecting int/any, got %s/%s\n", gettype($charset->state), $charset->state);

    mysqli_close($link);

    try {
        mysqli_get_charset($link);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--EXPECT--
mysqli object is already closed
done!

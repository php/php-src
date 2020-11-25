--TEST--
int mysqli_poll() and kill
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('connect.inc');
require_once('skipifconnectfailure.inc');

if (!$IS_MYSQLND)
    die("skip mysqlnd only feature, compile PHP using --with-mysqli=mysqlnd");
?>
--FILE--
<?php
    require_once('connect.inc');

    function get_connection() {
        global $host, $user, $passwd, $db, $port, $socket;

        if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
            printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
        return $link;
    }

    // Killing connection - 1

    $link = get_connection();
    if (true !== ($tmp = mysqli_query($link, "SELECT 1 AS 'processed before killed'", MYSQLI_ASYNC |  MYSQLI_USE_RESULT)))
        printf("[002] Expecting boolean/true got %s/%s\n", gettype($tmp), var_export($tmp, true));

    // Sleep 0.1s - the asynchronous query should have been processed after the wait period
    usleep(100000);
    $thread_id = mysqli_thread_id($link);
    mysqli_kill(get_connection(), $thread_id);

    $links = array($link);
    $errors = array($link);
    $reject = array($link);

    // Yes, 1 - the asynchronous query should have been processed
    if (1 !== ($tmp = (mysqli_poll($links, $errors, $reject, 0, 10000))))
        printf("[003] Expecting int/1 got %s/%s\n", gettype($tmp), var_export($tmp, true));

    if (!is_array($links) || empty($links))
        printf("[004] Expecting non-empty array got %s/%s\n", gettype($links), var_export($links, true));
    else
        foreach ($links as $link) {
            if (is_object($res = mysqli_reap_async_query($link))) {
                // Yes, you can fetch a result - the query has been processed
                var_dump(mysqli_fetch_assoc($res));
                mysqli_free_result($res);
            } else if ($link->errno > 0) {
                printf("[005] Error: %d\n", $link->errno);
            }
        }

    // No error!
    if (!is_array($errors) || !empty($errors))
        printf("[006] Expecting non-empty array got %s/%s\n", gettype($errors), var_export($errors, true));

    if (!is_array($reject) || !empty($reject))
        printf("[007] Expecting empty array got %s/%s\n", gettype($reject), var_export($reject, true));

    // Lets pass a dead connection
    $links = array($link);
    $errors = array($link);
    $reject = array($link);
    if (0 !== ($tmp = mysqli_poll($links, $errors, $reject, 1)))
        printf("[008] There should be no connection ready! Returned %s/%s, expecting int/0.\n",
            gettype($tmp), var_export($tmp, true));

    if (!empty($errors))
        printf("[009] There should be no errors but one rejected connection\n");

    foreach ($reject as $mysqli)
        if (mysqli_thread_id($mysqli) != $thread_id) {
            printf("[010] Rejected thread %d should have rejected thread %d\n",
                mysqli_thread_id($mysqli), $thread_id);
        }

    // Killing connection - 2

    $link = get_connection();
    if (true !== ($tmp = mysqli_query($link, "SELECT 1", MYSQLI_ASYNC |  MYSQLI_USE_RESULT)))
        printf("[011] Expecting boolean/true got %s/%s\n", gettype($tmp), var_export($tmp, true));

    usleep(100000);
    $thread_id = mysqli_thread_id($link);
    mysqli_kill(get_connection(), $thread_id);

    // Yes, 1 - fetch OK packet of kill!
    $processed = 0;
    $begin = microtime(true);
    do {
        $links = array($link, $link);
        $errors = array($link, $link);
        $reject = array($link, $link);
        $ready = mysqli_poll($links, $errors, $reject, 1);

        if (!empty($errors)) {
            foreach ($errors as $mysqli) {
                printf("[012] Error on thread %d: %s/%s\n",
                    mysqli_thread_id($mysqli),
                    mysqli_errno($mysqli),
                    mysqli_error($mysqli));
            }
            break;
        }

        if (FALSE === $ready) {
            printf("[013] MySQLi indicates some error\n");
            break;
        }

        if (!empty($reject)) {
            foreach ($reject as $mysqli) {
                printf("[014] Rejecting thread %d: %s/%s\n",
                    mysqli_thread_id($mysqli),
                    mysqli_errno($mysqli),
                    mysqli_error($mysqli));
            }
            $processed += count($reject);
        }

        foreach ($links as $mysqli) {
            if (is_object($res = mysqli_reap_async_query($mysqli))) {
                printf("Fetching from thread %d...\n", mysqli_thread_id($mysqli));
                var_dump(mysqli_fetch_assoc($res));
            } else if (mysqli_errno($mysqli) > 0) {
                printf("[015] %d/%s\n", mysqli_errno($mysqli), mysqli_error($mysqli));
            }
            $processed++;
        }

        if ((microtime(true) - $begin) > 5) {
            printf("[016] Pulling the emergency break after 5s, something is wrong...\n");
            break;
        }

    } while ($processed < 2);


    // Killing connection - 3

    $link = get_connection();
    $thread_id = mysqli_thread_id($link);
    mysqli_kill(get_connection(), $thread_id);
    // Sleep 0.1s  to ensure the KILL gets recognized
    usleep(100000);
    if (false !== ($tmp = mysqli_query($link, "SELECT 1 AS 'processed before killed'", MYSQLI_ASYNC |  MYSQLI_USE_RESULT)))
        printf("[017] Expecting boolean/false got %s/%s\n", gettype($tmp), var_export($tmp, true));

    $links = array($link);
    $errors = array($link);
    $reject = array($link);

    if (0 !== ($tmp = (mysqli_poll($links, $errors, $reject, 0, 10000))))
        printf("[018] Expecting int/0 got %s/%s\n", gettype($tmp), var_export($tmp, true));

    if (!is_array($links) || empty($links))
        printf("[019] Expecting non-empty array got %s/%s\n", gettype($links), var_export($links, true));
    else
        foreach ($links as $link) {
            if (is_object($res = mysqli_reap_async_query($link))) {
                // No, you cannot fetch the result
                var_dump(mysqli_fetch_assoc($res));
                mysqli_free_result($res);
            } else if ($link->errno > 0) {
                // But you are supposed to handle the error the way its shown here!
                printf("[020] Error: %d/%s\n", $link->errno, $link->error);
            }
        }

    // None of these will indicate an error, check errno on the list of returned connections!
    if (!is_array($errors) || !empty($errors))
        printf("[021] Expecting non-empty array got %s/%s\n", gettype($errors), var_export($errors, true));

    if (!is_array($reject) || !empty($reject))
        printf("[021] Expecting empty array got %s/%s\n", gettype($reject), var_export($reject, true));


    mysqli_close($link);
    print "done!";
?>
--XFAIL--
To be fixed later. Minor issue about fetching error message from killed line
--EXPECTF--
array(1) {
  ["processed before killed"]=>
  string(1) "1"
}
Fetching from thread %d...
array(1) {
  [1]=>
  string(1) "1"
}

Warning: mysqli_reap_async_query(): Premature end of data (mysqlnd_wireprotocol.c:%d) in %s on line %d

Warning: mysqli_reap_async_query(): RSET_HEADER %s

Warning: mysqli_reap_async_query(): Error reading result set's header in %s on line %d

Warning: Error while sending QUERY packet. %s

Warning: mysqli_reap_async_query(): %s

Warning: mysqli_reap_async_query(): Error reading result set's header in %s on line %d
[018] Error: %d/%s
done!

--TEST--
Calling connect() on an open persistent connection to create a new persistent connection
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--INI--
mysqli.allow_persistent=1
mysqli.max_persistent=-1
mysqli.max_links=-1
--FILE--
<?php
    require_once 'connect.inc';

    $phost = 'p:' . $host;

    if (!$link = my_mysqli_connect($phost, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $phost, $user, $db, $port, $socket);

    if (!$thread_id = $link->thread_id)
        printf("[002] Cannot determine thread id, test will fail, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (true !== ($tmp = my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)))
        printf("[003] Expecting boolean/true got %s/%s\n", gettype($tmp), $tmp);

    if (!is_int($new_thread_id = mysqli_thread_id($link)) || ($new_thread_id < 0))
        printf("[004] Expecting int/any got %s/%s\n", gettype($tmp), $tmp);

    if ($thread_id == $new_thread_id)
        printf("[005] Expecting new connection and new thread id. Old thread id %d, new thread id %d\n", $thread_id, $new_thread_id);

    if (!($res = mysqli_query($link, "SELECT 'ok' AS it_works")) ||
        !($row = mysqli_fetch_assoc($res)))
        printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    var_dump($row);
    mysqli_free_result($res);

    mysqli_close($link);

    $link = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

    if (!$thread_id = $link->thread_id)
        printf("[008] Cannot determine thread id, test will fail, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (true !== ($tmp = $link->real_connect($host, $user, $passwd, $db, $port, $socket)))
        printf("[009] Expecting boolean/true got %s/%s\n", gettype($tmp), $tmp);

    if (!is_int($new_thread_id = $link->thread_id) || ($new_thread_id < 0))
        printf("[010] Expecting int/any got %s/%s\n", gettype($tmp), $tmp);

    if ($thread_id == $new_thread_id)
        printf("[011] Expecting new connection and new thread id. Old thread id %d, new thread id %d\n", $thread_id, $new_thread_id);

    if (!($res = $link->query("SELECT 'works also with oo' AS syntax")) ||
            !($row = $res->fetch_assoc()))
        printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    var_dump($row);
    mysqli_free_result($res);

    mysqli_close($link);

    if (true !== ($tmp = $link->connect($phost, $user, $passwd, $db, $port, $socket)))
        printf("[013] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);

    if (!$link = mysqli_connect($phost, $user, $passwd, $db, $port, $socket))
        printf("[014] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $phost, $user, $db, $port, $socket);

    if (true !== ($tmp = $link->connect($host, $user, $passwd, $db, $port, $socket)))
        printf("[015] Expecting true got %s/%s\n", gettype($tmp), $tmp);

    printf("Flipping phost/host order\n");

        if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[016] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (!$thread_id = mysqli_thread_id($link))
        printf("[017] Cannot determine thread id, test will fail, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (true !== ($tmp = my_mysqli_real_connect($link, $phost, $user, $passwd, $db, $port, $socket)))
        printf("[018] Expecting boolean/true got %s/%s\n", gettype($tmp), $tmp);

    if (!is_int($new_thread_id = mysqli_thread_id($link)) || ($new_thread_id < 0))
        printf("[019] Expecting int/any got %s/%s\n", gettype($tmp), $tmp);

    if ($thread_id == $new_thread_id)
        printf("[020] Expecting new connection and new thread id. Old thread id %d, new thread id %d\n", $thread_id, $new_thread_id);

    if (!($res = mysqli_query($link, "SELECT 'ok' AS it_works")) ||
        !($row = mysqli_fetch_assoc($res)))
        printf("[021] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    var_dump($row);
    mysqli_free_result($res);

    mysqli_close($link);

    $link = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

    if (!$thread_id = $link->thread_id)
        printf("[023] Cannot determine thread id, test will fail, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (true !== ($tmp = $link->real_connect($phost, $user, $passwd, $db, $port, $socket)))
        printf("[024] Expecting boolean/true got %s/%s\n", gettype($tmp), $tmp);

    if (!is_int($new_thread_id = $link->thread_id) || ($new_thread_id < 0))
        printf("[025] Expecting int/any got %s/%s\n", gettype($tmp), $tmp);

    if ($thread_id == $new_thread_id)
        printf("[026] Expecting new connection and new thread id. Old thread id %d, new thread id %d\n", $thread_id, $new_thread_id);

    if (!($res = $link->query("SELECT 'works also with oo' AS syntax")) ||
            !($row = $res->fetch_assoc()))
        printf("[027] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    var_dump($row);
    mysqli_free_result($res);

    mysqli_close($link);

    if (true !== ($tmp = $link->connect($host, $user, $passwd, $db, $port, $socket)))
        printf("[028] Expecting true got %s/%s\n", gettype($tmp), $tmp);

    if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[029] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (true !== ($tmp = $link->connect($phost, $user, $passwd, $db, $port, $socket)))
        printf("[030] Expecting true got %s/%s\n", gettype($tmp), $tmp);

    print "done!";
?>
--EXPECT--
array(1) {
  ["it_works"]=>
  string(2) "ok"
}
array(1) {
  ["syntax"]=>
  string(18) "works also with oo"
}
Flipping phost/host order
array(1) {
  ["it_works"]=>
  string(2) "ok"
}
array(1) {
  ["syntax"]=>
  string(18) "works also with oo"
}
done!

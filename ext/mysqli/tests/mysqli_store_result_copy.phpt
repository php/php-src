--TEST--
mysqli_store_result()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
if (!$IS_MYSQLND) {
    die("SKIP mysqlnd only test");
}
?>
--INI--
mysqlnd.debug="d:t:O,{TMP}/mysqlnd.trace"
mysqlnd.net_read_buffer_size=1
mysqlnd.mempool_default_size=1
mysqlnd.fetch_data_copy=0
--FILE--
<?php
    require_once("connect.inc");

    $tmp    = NULL;
    $link   = NULL;

    require('table.inc');

    if (!$res = mysqli_real_query($link, "SELECT id, label FROM test ORDER BY id"))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!is_object($res = mysqli_store_result($link, MYSQLI_STORE_RESULT_COPY_DATA)))
        printf("[004] Expecting object, got %s/%s. [%d] %s\n",
            gettype($res), $res, mysqli_errno($link), mysqli_error($link));

    if (true !== ($tmp = mysqli_data_seek($res, 2)))
        printf("[005] Expecting boolean/true, got %s/%s. [%d] %s\n",
            gettype($tmp), $tmp, mysqli_errno($link), mysqli_error($link));

    var_dump($res->fetch_assoc());

    if (true !== ($tmp = mysqli_data_seek($res, 0)))
        printf("[006] Expecting boolean/true, got %s/%s. [%d] %s\n",
            gettype($tmp), $tmp, mysqli_errno($link), mysqli_error($link));

    while ($row = $res->fetch_assoc()) {
        printf("id = %d, label = %s\n", $row['id'], $row['label']);
    }

    mysqli_free_result($res);
    mysqli_close($link);

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[007] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
    }


    if (!$res = mysqli_real_query($link, "SELECT id, label FROM test ORDER BY id"))
        printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!is_object($res = mysqli_store_result($link, MYSQLI_STORE_RESULT_COPY_DATA)))
        printf("[009] Expecting object, got %s/%s. [%d] %s\n",
            gettype($res), $res, mysqli_errno($link), mysqli_error($link));

    $no_result = 0;
    for ($i = 0; $i < 1000; $i++) {
        $idx = mt_rand(-100, 100);
        if (true === @mysqli_data_seek($res, $idx)) {
            $row = $res->fetch_assoc();
            if (!isset($row['id']) || !isset($row['label'])) {
                printf("[010] Brute force seek %d returned %d\n", $idx, var_export($row, true));
            }
        } else {
            $no_result++;
        }
    }
    printf("No result: %d\n", $no_result);

    /* implicit free, implicit store */
    /* meta and fetch lengths code */
    if (!$res = mysqli_query($link, "SELECT CONCAT(id, id) AS _c, label FROM test ORDER BY id DESC LIMIT 2"))
        printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    printf("Default\n");
    var_dump(mysqli_fetch_lengths($res));
    $fields = $res->fetch_fields();
    while ($row = $res->fetch_assoc()) {
        var_dump(mysqli_fetch_lengths($res));
    }
    var_dump(mysqli_fetch_lengths($res));

    if (!$res = mysqli_real_query($link, "SELECT CONCAT(id, id) AS _c, label FROM test ORDER BY id DESC LIMIT 2"))
        printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!is_object($res = mysqli_store_result($link, MYSQLI_STORE_RESULT_COPY_DATA)))
        printf("[013] Expecting object, got %s/%s. [%d] %s\n",
            gettype($res), $res, mysqli_errno($link), mysqli_error($link));

    printf("Copy\n");
    var_dump(mysqli_fetch_lengths($res));
    $fields_copy = $res->fetch_fields();
    while ($row = $res->fetch_assoc()) {
        var_dump(mysqli_fetch_lengths($res));
    }
    var_dump(mysqli_fetch_lengths($res));

    /* There's no need for in-depth testing here. If you want in-depth switch mysqlnd
    globally to copy mode and run all the tests */
    foreach ($fields as $k => $field_info) {
        if ($fields_copy[$k] != $field_info) {
            printf("[014] Metadata seems to differ, dumping\n");
            var_dump($field_info);
            var_dump($fields_copy[$k]);
        }
    }

    /* fetch all */

    if (!$res = mysqli_real_query($link, "SELECT id, label FROM test ORDER BY id DESC LIMIT 2"))
        printf("[015] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!is_object($res = mysqli_store_result($link, MYSQLI_STORE_RESULT_COPY_DATA)))
        printf("[016] Expecting object, got %s/%s. [%d] %s\n",
            gettype($res), $res, mysqli_errno($link), mysqli_error($link));

    foreach (mysqli_fetch_all($res, MYSQLI_ASSOC) as $row) {
        printf("id = %d label = %s\n", $row['id'], $row['label']);
    }

    if (!$res = mysqli_real_query($link, "SELECT id, label FROM test ORDER BY id DESC LIMIT 2"))
        printf("[017] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!is_object($res = mysqli_store_result($link, MYSQLI_STORE_RESULT_COPY_DATA)))
        printf("[018] Expecting object, got %s/%s. [%d] %s\n",
            gettype($res), $res, mysqli_errno($link), mysqli_error($link));

    /* provoke out of sync */
    if (!mysqli_real_query($link, "SELECT id, label FROM test ORDER BY id DESC LIMIT 2"))
        printf("[019] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    var_dump($res->fetch_assoc());

    if (!$res = mysqli_real_query($link, "SELECT id, label FROM test ORDER BY id ASC LIMIT 2"))
        printf("[020] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!is_object($res = mysqli_store_result($link, MYSQLI_STORE_RESULT_COPY_DATA)))
        printf("[021] Expecting object, got %s/%s. [%d] %s\n",
            gettype($res), $res, mysqli_errno($link), mysqli_error($link));

    /* user conn killed, res associated with conn, fetch from res */
    unset($link);
    var_dump($res->fetch_assoc());


    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[022] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
    }

    if (!$res = mysqli_real_query($link, "INSERT INTO test(id, label) VALUES (100, 'z')"))
        printf("[023] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    mysqli_store_result($link, MYSQLI_STORE_RESULT_COPY_DATA);

    if (mysqli_get_server_version($link) > 50000) {
        // let's try to play with stored procedures
        mysqli_real_query($link, 'DROP PROCEDURE IF EXISTS p');
        if (mysqli_real_query($link, 'CREATE PROCEDURE p(OUT ver_param VARCHAR(25)) READS SQL DATA BEGIN SELECT id FROM test WHERE id >= 100 ORDER BY id; SELECT id + 1, label FROM test WHERE id > 0 AND id < 3 ORDER BY id; SELECT VERSION() INTO ver_param;
END;')) {
            mysqli_multi_query($link, "CALL p(@version)");
            do {
                if ($res = $link->store_result(MYSQLI_STORE_RESULT_COPY_DATA)) {
                    printf("---\n");
                    var_dump($res->fetch_all());
                    $res->free();
                } else {
                    if ($link->errno) {
                        echo "Store failed: (" . $link->errno . ") " . $link->error;
                    }
                }
            } while ($link->more_results() && $link->next_result());
            mysqli_real_query($link, 'SELECT @version AS p_version');
            $res = mysqli_store_result($link, MYSQLI_STORE_RESULT_COPY_DATA);

            $tmp = mysqli_fetch_assoc($res);
            if (!is_array($tmp) || empty($tmp) || !isset($tmp['p_version']) || ('' == $tmp['p_version'])) {
                printf("[024] Expecting array [%d] %s\n", mysqli_errno($link), mysqli_error($link));
                var_dump($tmp);
            }

            mysqli_free_result($res);
        } else {
                printf("[025] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
        }
    }

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
array(2) {
  ["id"]=>
  string(1) "3"
  ["label"]=>
  string(1) "c"
}
id = 1, label = a
id = 2, label = b
id = 3, label = c
id = 4, label = d
id = 5, label = e
id = 6, label = f
No result: %d
Default
bool(false)
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(1)
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(1)
}
bool(false)
Copy
bool(false)
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(1)
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(1)
}
bool(false)
id = 6 label = f
id = 5 label = e
array(2) {
  ["id"]=>
  string(1) "6"
  ["label"]=>
  string(1) "f"
}
[020] [2014] %s
array(2) {
  ["id"]=>
  string(1) "6"
  ["label"]=>
  string(1) "f"
}
---
array(1) {
  [0]=>
  array(1) {
    [0]=>
    string(3) "100"
  }
}
---
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "2"
    [1]=>
    string(1) "a"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "3"
    [1]=>
    string(1) "b"
  }
}
done!

--TEST--
API vs. SQL LAST_INSERT_ID()
--SKIPIF--
<?php
  require_once('skipif.inc');
  require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    /*
    CAUTION: the insert_id() API call is not supposed to return
    the same value as a call to the LAST_INSERT_ID() SQL function.
    It is not necessarily a bug if API and SQL function return different
    values. Check the MySQL C API reference manual for details.
    */
    require_once("connect.inc");

    function get_sql_id($link) {
        if (!($res = $link->query("SELECT LAST_INSERT_ID() AS _id"))) {
            printf("[003] [%d] %s\n", $link->errno, $link->error);
            return NULL;
        }
        $row = $res->fetch_assoc();
        $res->close();

        return $row['_id'];
    }

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (!$link->query("DROP TABLE IF EXISTS test") ||
        !$link->query("CREATE TABLE test (id INT auto_increment, label varchar(10) not null, PRIMARY KEY (id)) ENGINE=MyISAM") ||
        !$link->query("INSERT INTO test (id, label) VALUES (null, 'a')")) {
        printf("[002] [%d] %s\n", $link->errno, $link->error);
    }

    $api_id = $link->insert_id;
    $sql_id = get_sql_id($link);
    printf("API: %d, SQL: %d\n", $api_id, $sql_id);

    if ($api_id < 1)
        printf("[004] Expecting id > 0 got %d, [%d] %s\n", $api_id, $link->errno, $link->error)	;
    if ($api_id != $sql_id)
        printf("[005] SQL id %d should be equal to API id %d\n", $sql_id, $api_id);

    // Not an INSERT, API value must become 0
    if (!($res = $link->query("SELECT 1 FROM DUAL")))
        printf("[006] [%d] %s\n", $link->errno, $link->error);
    else
        $res->close();

    $api_id = $link->insert_id;
    $new_sql_id = get_sql_id($link);
    if (0 !== $api_id) {
        printf("[007] API id should have been reset to 0 because previous query was SELECT, got API %d, SQL %d\n",
          $api_id, $new_sql_id);
    }
    if ($new_sql_id != $sql_id) {
        printf("[008] The servers LAST_INSERT_ID() changed unexpectedly from %d to %d\n", $sql_id, $new_sql_id);
    }

    // Insert fails, LAST_INSERT_ID shall not change, API shall return 0
    if ($link->query("INSERT INTO test (id, label) VALUES (null, null)")) {
        printf("[009] The INSERT did not fail as planned, [%d] %s\n", $link->errno, $link->error);
    }
    $api_id = $link->insert_id;
    $new_sql_id = get_sql_id($link);

    if (0 !== $api_id) {
        printf("[010] API id should have been reset to 0 because previous query was SELECT, got API %d, SQL %d\n",
          $api_id, $new_sql_id);
    }
    if ($new_sql_id != $sql_id) {
        printf("[011] The servers LAST_INSERT_ID() changed unexpectedly from %d to %d\n", $sql_id, $new_sql_id);
    }

    // Sequence counter pattern...
    if (!$link->query("UPDATE test SET id=LAST_INSERT_ID(id+1)"))
      printf("[012] [%d] %s\n", $link->errno, $link->error);

    $api_id = $link->insert_id;
    $new_sql_id = get_sql_id($link);
    if ($api_id < 1)
        printf("[013] Expecting id > 0 got %d, [%d] %s\n", $api_id, $link->errno, $link->error)	;
    if ($api_id != $new_sql_id)
        printf("[014] SQL id %d should be equal to API id %d\n", $new_sql_id, $api_id);
    if ($sql_id == $new_sql_id)
        printf("[015] SQL id %d should have had changed, got %d\n", $sql_id, $new_sql_id);

    $sql_id = $new_sql_id;

    // Not an INSERT (after UPDATE), API value must become 0
    if (!$link->query("SET @myvar=1"))
        printf("[016] [%d] %s\n", $link->errno, $link->error);

    $api_id = $link->insert_id;
    $new_sql_id = get_sql_id($link);
    if (0 !== $api_id) {
        printf("[017] API id should have been reset to 0 because previous query was SET, got API %d, SQL %d\n",
          $api_id, $new_sql_id);
    }
    if ($new_sql_id != $sql_id) {
        printf("[018] The servers LAST_INSERT_ID() changed unexpectedly from %d to %d\n", $sql_id, $new_sql_id);
    }

    if (!$link->query("INSERT INTO test(id, label) VALUES (LAST_INSERT_ID(id + 1), 'b')"))
        printf("[019] [%d] %s\n", $link->errno, $link->error);

    $api_id = $link->insert_id;
    $sql_id = get_sql_id($link);
    if ($api_id != $sql_id)
        printf("[020] SQL id %d should be equal to API id %d\n", $sql_id, $api_id);

    if (!$link->query("INSERT INTO test(label) VALUES ('c')"))
        printf("[021] [%d] %s\n", $link->errno, $link->error);

    $api_id = $link->insert_id;
    $sql_id = get_sql_id($link);
    if ($api_id != $sql_id)
        printf("[022] SQL id %d should be equal to API id %d\n", $sql_id, $api_id);

    if (!($res = $link->query("SELECT id, label FROM test ORDER BY id ASC")))
        printf("[023] [%d] %s\n", $link->errno, $link->error);

    printf("Dumping table contents before INSERT...SELECT experiments...\n");
    while ($row = $res->fetch_assoc()) {
        printf("id = %d, label = '%s'\n", $row['id'], $row['label']);
    }
    $res->close();

    if (!$link->query("INSERT INTO test(label) SELECT CONCAT(label, id) FROM test ORDER BY id ASC"))
      printf("[024] [%d] %s\n", $link->errno, $link->error);

    $api_id = $link->insert_id;
    $sql_id = get_sql_id($link);
    if ($api_id != $sql_id)
        printf("[025] SQL id %d should be equal to API id %d\n", $sql_id, $api_id);

    if ($link->query("INSERT INTO test(id, label) SELECT id, CONCAT(label, id) FROM test ORDER BY id ASC"))
      printf("[026] INSERT should have failed because of duplicate PK value, [%d] %s\n", $link->errno, $link->error);

    $api_id = $link->insert_id;
    $new_sql_id = get_sql_id($link);
    if (0 !== $api_id) {
        printf("[027] API id should have been reset to 0 because previous query failed, got API %d, SQL %d\n",
          $api_id, $new_sql_id);
    }
    if ($new_sql_id != $sql_id) {
        printf("[028] The servers LAST_INSERT_ID() changed unexpectedly from %d to %d\n", $sql_id, $new_sql_id);
    }

    /* API insert id will be 101 because of UPDATE, SQL unchanged */
    if (!$link->query(sprintf("INSERT INTO test(id, label) VALUES (%d, 'z') ON DUPLICATE KEY UPDATE id = 101", $sql_id)	))
      printf("[029] [%d] %s\n", $link->errno, $link->error);

    $api_id = $link->insert_id;
    $new_sql_id = get_sql_id($link);
    if ($api_id != 101)
        printf("[030] API id should be %d got %d\n", $sql_id, $api_id);
    if ($new_sql_id != $sql_id) {
        printf("[031] The servers LAST_INSERT_ID() changed unexpectedly from %d to %d\n", $sql_id, $new_sql_id);
    }

    if (!($res = $link->query("SELECT id, label FROM test ORDER BY id ASC")))
        printf("[032] [%d] %s\n", $link->errno, $link->error);

    printf("Dumping table contents after INSERT...SELECT...\n");
    while ($row = $res->fetch_assoc()) {
        printf("id = %d, label = '%s'\n", $row['id'], $row['label']);
    }
    $res->close();

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
API: %d, SQL: %d
Dumping table contents before INSERT...SELECT experiments...
id = %d, label = 'b'
id = %d, label = 'a'
id = %d, label = 'c'
Dumping table contents after INSERT...SELECT...
id = %d, label = 'b'
id = %d, label = 'a'
id = %d, label = 'c'
id = %d, label = 'a%d'
id = %d, label = 'c%d'
id = 101, label = 'b%d'
done!

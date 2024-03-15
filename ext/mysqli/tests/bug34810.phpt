--TEST--
Bug #34810 (mysqli::init() and others use wrong $this pointer without checks)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

class DbConnection {
    public function connect() {
        require_once 'connect.inc';

        /* Pass false as $connect_flags cannot be accessed via globals. */
        $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket, false);
        var_dump($link);

        $link = mysqli_init();
        var_dump($link);

        $mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket, false);
        $mysql->query("DROP TABLE IF EXISTS test_warnings");
        $mysql->query("CREATE TABLE test_warnings (a int not null)");
        $mysql->query("SET sql_mode=''");
        $mysql->query("INSERT INTO test_warnings VALUES (1),(2),(NULL)");

        $warning = $mysql->get_warnings();
        if (!$warning)
            printf("[001] No warning!\n");

        if ($warning->errno == 1048 || $warning->errno == 1253) {
            /* 1048 - Column 'a' cannot be null, 1263 - Data truncated; NULL supplied to NOT NULL column 'a' at row */
            if ("HY000" != $warning->sqlstate)
                printf("[003] Wrong sql state code: %s\n", $warning->sqlstate);

            if ("" == $warning->message)
                printf("[004] Message string must not be empty\n");
        } else {
            printf("[002] Empty error message!\n");
            var_dump($warning);
        }
    }
}

$db = new DbConnection();
$db->connect();

echo "Done\n";
?>
--CLEAN--
<?php
require_once 'connect.inc';
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_warnings"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECTF--
object(mysqli)#%d (%d) {
  ["affected_rows"]=>
  int(0)
  ["client_info"]=>
  string(%d) "%s"
  ["client_version"]=>
  int(%d)
  ["connect_errno"]=>
  int(0)
  ["connect_error"]=>
  NULL
  ["errno"]=>
  int(0)
  ["error"]=>
  string(0) ""
  ["error_list"]=>
  array(0) {
  }
  ["field_count"]=>
  int(0)
  ["host_info"]=>
  string(%d) "%s"
  ["info"]=>
  %s
  ["insert_id"]=>
  int(0)
  ["server_info"]=>
  string(%d) "%s"
  ["server_version"]=>
  int(%d)
  ["sqlstate"]=>
  string(5) "00000"
  ["protocol_version"]=>
  int(10)
  ["thread_id"]=>
  int(%d)
  ["warning_count"]=>
  int(0)
}
object(mysqli)#%d (%d) {
  ["client_info"]=>
  string(%d) "%s"
  ["client_version"]=>
  int(%d)
  ["connect_errno"]=>
  int(0)
  ["connect_error"]=>
  NULL
  ["errno"]=>
  int(0)
  ["error"]=>
  string(0) ""
}
Done

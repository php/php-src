--TEST--
Fail gracefully on empty result set
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

    // Returns only one result set
    $link->multi_query("SELECT 1");
    var_dump($link->next_result()); // should return false
    var_dump($link->store_result()); // now what happens here!?


    // Returns only one result set
    $link->multi_query("SELECT 1");
    var_dump($link->next_result());
    var_dump($link->use_result());

    $link->close();
?>
--EXPECT--
bool(false)
object(mysqli_result)#3 (5) {
  ["current_field"]=>
  int(0)
  ["field_count"]=>
  int(1)
  ["lengths"]=>
  NULL
  ["num_rows"]=>
  int(1)
  ["type"]=>
  int(0)
}
bool(false)
object(mysqli_result)#3 (5) {
  ["current_field"]=>
  int(0)
  ["field_count"]=>
  int(1)
  ["lengths"]=>
  NULL
  ["num_rows"]=>
  int(0)
  ["type"]=>
  int(1)
}

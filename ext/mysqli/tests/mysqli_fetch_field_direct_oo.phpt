--TEST--
$res->fetch_field_direct(s)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $tmp    = NULL;
    $link   = NULL;

    $mysqli = new mysqli();
    $res = @new mysqli_result($mysqli);
    if (!is_null($tmp = @$res->fetch_field_direct()))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    require('table.inc');

    if (!$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket))
        printf("[002] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (!$res = $mysqli->query("SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 1")) {
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    if (!is_null($tmp = @$res->fetch_field_direct()))
        printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!is_null($tmp = @$res->fetch_field_direct($link)))
        printf("[005] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!is_null($tmp = @$res->fetch_field_direct($link, $link)))
        printf("[006] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    var_dump($res->fetch_field_direct(-1));
    var_dump($res->fetch_field_direct(0));
    var_dump($res->fetch_field_direct(2));

    $res->free_result();

    if (false !== ($tmp = $res->fetch_field_direct(0)))
        printf("[007] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    $mysqli->close();
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_result::fetch_field_direct(): Field offset is invalid for resultset in %s on line %d
bool(false)
object(stdClass)#%d (13) {
  ["name"]=>
  string(2) "ID"
  ["orgname"]=>
  string(2) "id"
  ["table"]=>
  string(4) "TEST"
  ["orgtable"]=>
  string(%d) "%s"
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(%d) "%s"
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(%d)
  ["length"]=>
  int(11)
  ["charsetnr"]=>
  int(%d)
  ["flags"]=>
  int(%d)
  ["type"]=>
  int(%d)
  ["decimals"]=>
  int(%d)
}

Warning: mysqli_result::fetch_field_direct(): Field offset is invalid for resultset in %s on line %d
bool(false)

Warning: mysqli_result::fetch_field_direct(): Couldn't fetch mysqli_result in %s on line %d
done!

--TEST--
Bug #36802 (crashes with with mysqli_set_charset())
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
    class really_my_mysqli extends mysqli {
        function __construct()
        {
        }
    }

    require_once("connect.inc");
    $mysql = mysqli_init();

    /* following operations should not work */
    if (method_exists($mysql, 'set_charset')) {
        $x[0] = @$mysql->set_charset('utf8');
    } else {
        $x[0] = false;
    }
    $x[1] = @$mysql->query("SELECT 'foo' FROM DUAL");

    /* following operations should work */
    $x[2] = ($mysql->client_version > 0);
    $x[3] = $mysql->errno;
    $mysql->close();

    var_dump($x);
?>
--EXPECT--
array(4) {
  [0]=>
  bool(false)
  [1]=>
  bool(false)
  [2]=>
  bool(true)
  [3]=>
  int(0)
}

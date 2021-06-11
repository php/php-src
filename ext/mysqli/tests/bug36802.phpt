--TEST--
Bug #36802 (crashes with with mysqli_set_charset())
--EXTENSIONS--
mysqli
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
        try {
            $mysql->set_charset('utf8');
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }
    } else {
        $x[0] = false;
    }

    try {
        $mysql->query("SELECT 'foo' FROM DUAL");
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    /* following operations should work */
    $x[1] = ($mysql->error);
    $x[2] = $mysql->errno;

    $mysql->close();

    var_dump($x);
?>
--EXPECT--
mysqli object is not fully initialized
mysqli object is not fully initialized
array(2) {
  [1]=>
  string(0) ""
  [2]=>
  int(0)
}

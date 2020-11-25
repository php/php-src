--TEST--
mysqli_result(), invalid mode
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require('connect.inc');
    require('table.inc');

    $valid = array(MYSQLI_STORE_RESULT, MYSQLI_USE_RESULT);
    do {
        $mode = mt_rand(-1000, 1000);
    } while (in_array($mode, $valid));

    if (!is_object($res = new mysqli_result($link, $mode)))
        printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_result::__construct(): Invalid value for resultmode in %s on line %d
done!

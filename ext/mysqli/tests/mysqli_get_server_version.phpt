--TEST--
mysqli_get_server_version()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    require "table.inc";
    /* 5.1.5 -> 50105 -- major_version*10000 + minor_version *100 + sub_version */
    /* < 30000 = pre 3.2.3, very unlikely! */
    if (!is_int($info = mysqli_get_server_version($link)) || ($info < (3 * 10000)))
        printf("[003] Expecting int/any >= 30000, got %s/%s\n", gettype($info), $info);

    print "done!";
?>
--EXPECT--
done!

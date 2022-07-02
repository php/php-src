--TEST--
gd_info()
--EXTENSIONS--
gd
--FILE--
<?php

    echo "basic test of gd_info() function\n";

    var_dump(gd_info());

    echo "\nDone\n";
?>
--EXPECTF--
basic test of gd_info() function
array(%d) {
%a
}

Done

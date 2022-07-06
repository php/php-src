--TEST--
Bug #20865 (array_key_exists and NULL key)
--FILE--
<?php
    $ta = array(1, 2, 3);
    $ta[NULL] = "Null Value";

    var_dump(array_key_exists(NULL, $ta));
?>
--EXPECT--
bool(true)

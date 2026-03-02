--TEST--
Bug #20865 (array_key_exists and NULL key)
--FILE--
<?php
    $ta = array(1, 2, 3);
    $ta[NULL] = "Null Value";

    var_dump(array_key_exists(NULL, $ta));
?>
--EXPECTF--
Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d

Deprecated: Using null as the key parameter for array_key_exists() is deprecated, use an empty string instead in %s on line %d
bool(true)

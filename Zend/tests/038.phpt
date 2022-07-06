--TEST--
Trying to use lambda as array key
--FILE--
<?php

try {
    var_dump(array(function() { } => 1));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Illegal offset type

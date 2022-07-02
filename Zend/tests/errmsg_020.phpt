--TEST--
errmsg: disabled function
--INI--
disable_functions=phpinfo
--FILE--
<?php

try {
    phpinfo();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Call to undefined function phpinfo()

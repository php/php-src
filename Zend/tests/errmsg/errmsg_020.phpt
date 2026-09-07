--TEST--
errmsg: disabled function
--INI--
disable_functions=phpinfo
--FILE--
<?php

try {
    phpinfo();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Call to undefined function phpinfo()

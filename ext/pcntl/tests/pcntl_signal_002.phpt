--TEST--
pcntl_signal() - If handler is an int value different than SIG_DFL or SIG_IGN
--EXTENSIONS--
pcntl
--FILE--
<?php

try {
    pcntl_signal(SIGTERM, -1);
} catch (Error $error) {
    echo $error->getMessage();
}

?>
--EXPECT--
pcntl_signal(): Argument #2 ($handler) must be either SIG_DFL or SIG_IGN when an integer value is given

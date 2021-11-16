--TEST--
readline_clear_history(): Basic test
--EXTENSIONS--
readline
--FILE--
<?php

var_dump(readline_clear_history());

?>
--EXPECT--
bool(true)

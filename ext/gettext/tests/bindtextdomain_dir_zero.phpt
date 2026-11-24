--TEST--
bindtextdomain() throws a ValueError when the directory is "0"
--EXTENSIONS--
gettext
--FILE--
<?php

try {
    bindtextdomain('app', '0');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

chdir(__DIR__);
var_dump(bindtextdomain('app', '') === realpath(__DIR__));

?>
--EXPECT--
bindtextdomain(): Argument #2 ($directory) cannot be zero
bool(true)

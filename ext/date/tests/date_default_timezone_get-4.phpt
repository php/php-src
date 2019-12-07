--TEST--
date_default_timezone_get() function [4]
--INI--
date.timezone=Incorrect/Zone
--FILE--
<?php
try {
    var_dump(date_default_timezone_get());
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
Invalid date.timezone value 'Incorrect/Zone'

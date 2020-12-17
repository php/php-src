--TEST--
php_user_filter onCreate() returns false
--FILE--
<?php

class my_filter extends php_user_filter {
    function onCreate() {
        return false;
    }
}

stream_filter_register("my_filter", "my_filter");
$fp = fopen('php://memory', 'rw');
var_dump(stream_filter_append($fp, "my_filter"));
fwrite($fp, "Test");
fseek($fp, 0);
var_dump(fgets($fp));
fclose($fp);

?>
--EXPECTF--
Warning: stream_filter_append(): Unable to create or locate filter "my_filter" in %s on line %d
bool(false)
string(4) "Test"

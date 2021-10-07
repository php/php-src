--TEST--
Bug #54350: Memory corruption with user_filter
--FILE--
<?php

class user_filter extends php_user_filter {
    function filter($in, $out, &$consumed, $closing): int {
        while ($bucket = stream_bucket_make_writeable($in)) {
        }
        fclose($this->stream);
        return 0;
    }
}
stream_filter_register('user_filter','user_filter');
$fd = fopen('php://memory','w');
$filter = stream_filter_append($fd, 'user_filter');
fwrite($fd, "foo");

?>
--EXPECTF--
Warning: fclose(): 5 is not a valid stream resource in %s on line %d

Warning: fclose(): supplied resource is not a valid stream resource in %s on line %d

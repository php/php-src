--TEST--
Bug #54350: Memory corruption with user_filter
--FILE--
<?php

class user_filter extends php_user_filter {
    function filter($in, $out, &$consumed, $closing): int {
        while ($bucket = stream_bucket_make_writeable($in)) {
        }
        try {
            fclose($this->stream);
        } catch (TypeError $e) {
            echo $e->getMessage(), "\n";
        }
        return 0;
    }
}
stream_filter_register('user_filter','user_filter');
$fd = fopen('php://memory','w');
$filter = stream_filter_append($fd, 'user_filter');
fwrite($fd, "foo");

?>
--EXPECTF--
Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d
fclose(): Argument #1 ($stream) must be an open stream resource

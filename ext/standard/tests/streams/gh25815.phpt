--TEST--
GH-25815 (heap-use-after-free when a userspace stream_stat() closes the stream)
--CREDITS--
snowjeon2
--FILE--
<?php
class W {
    public $context;
    function stream_open($path, $mode, $options, &$opened_path) { return true; }
    function stream_stat() { global $s; fclose($s); return []; }
    function stream_read($count) { return ''; }
    function stream_eof() { return true; }
}
stream_wrapper_register("bomb", "W");
$s = fopen("bomb://", "r");
var_dump(stream_get_contents($s));
?>
--EXPECTF--
Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d
string(0) ""

--TEST--
GH-25586 (heap-use-after-free in php_stream_cast() when a userspace stream_flush() closes the stream)
--CREDITS--
YuanchengJiang
--FILE--
<?php
class W {
    public $context;
    function stream_open($path, $mode, $options, &$opened_path) { return true; }
    function stream_flush() { global $s; fclose($s); return true; }
}
stream_wrapper_register("bomb", "W");
$s = fopen("bomb://", "w");
var_dump(proc_open("true", [1 => $s], $pipes));
?>
--EXPECTF--
Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d

Warning: proc_open(): W::stream_cast is not implemented! in %s on line %d

Warning: proc_open(): Cannot represent a stream of type user-space as a File Descriptor in %s on line %d
bool(false)

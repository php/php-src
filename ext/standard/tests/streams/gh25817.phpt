--TEST--
GH-25817 (Use-after-free when a userspace stream_truncate() closes the stream)
--CREDITS--
snowjeon2
--FILE--
<?php
class W {
    public $context;
    function stream_open($path, $mode, $options, &$opened_path) { return true; }
    function stream_truncate(int $new_size) { global $s; fclose($s); return 1; }
}
stream_wrapper_register("bomb", "W");
$s = fopen("bomb://", "w");
var_dump(ftruncate($s, 0));
?>
--EXPECTF--
Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d

Warning: ftruncate(): W::stream_truncate value must be of type bool, int given in %s on line %d
bool(false)

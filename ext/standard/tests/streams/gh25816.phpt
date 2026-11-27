--TEST--
GH-25816 (stream_lock() can close the stream while it is in use)
--CREDITS--
snowjeon2
--FILE--
<?php
class W {
    public $context;
    function stream_open($path, $mode, $options, &$opened_path) { return true; }
    function stream_lock($operation) { global $s; fclose($s); return true; }
}
stream_wrapper_register("lock", "W");
$s = fopen("lock://", "r");
var_dump(flock($s, LOCK_EX));
var_dump(is_resource($s));
?>
--EXPECTF--
Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d
bool(true)
bool(true)

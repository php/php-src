--TEST--
GH-21700 (_php_stream_seek assertion on SEEK_CUR with resolved negative offset)
--CREDITS--
YuanchengJiang
--FILE--
<?php
class wrapper {
    public int $pos = 0;
    public $context;
    function stream_open($path, $mode, $options, &$opened_path): bool { return true; }
    function stream_seek($offset, $whence): bool { $this->pos = $offset; return true; }
    function stream_tell(): int { return $this->pos; }
}
stream_wrapper_register("gh21700", "wrapper");
$fp = fopen("gh21700://", "rb");
var_dump(fseek($fp, -1, SEEK_CUR));
var_dump(fseek($fp, 0, SEEK_CUR));
fclose($fp);
?>
--EXPECT--
int(-1)
int(0)

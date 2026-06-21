--TEST--
imageloadfont(): header read must stay in bounds on short reads
--EXTENSIONS--
gd
--FILE--
<?php
/* A user-space wrapper returns one byte per read, so php_stream_read() hands
 * imageloadfont()'s header loop a short read on every iteration. The header
 * (4 ints) plus a single 1x1 glyph byte form a valid font, which only loads
 * when each short read lands at the correct byte offset. */
class drip
{
    public $context;
    private string $data;
    private int $pos = 0;

    public function stream_open($path, $mode, $options, &$opened): bool
    {
        $this->data = pack('V4', 1, 32, 1, 1) . "\x00";
        return true;
    }

    public function stream_read($count): string
    {
        return $this->pos < strlen($this->data) ? $this->data[$this->pos++] : '';
    }

    public function stream_eof(): bool
    {
        return $this->pos >= strlen($this->data);
    }

    public function stream_stat()
    {
        return [];
    }

    public function stream_tell(): int
    {
        return $this->pos;
    }

    public function stream_seek($offset, $whence): bool
    {
        if ($whence === SEEK_CUR) {
            $this->pos += $offset;
        } elseif ($whence === SEEK_END) {
            $this->pos = strlen($this->data) + $offset;
        } else {
            $this->pos = $offset;
        }
        return true;
    }

    public function stream_set_option($option, $arg1, $arg2): bool
    {
        return false;
    }
}

stream_wrapper_register('drip', drip::class);
var_dump(imageloadfont('drip://font') instanceof GdFont);
?>
--EXPECT--
bool(true)

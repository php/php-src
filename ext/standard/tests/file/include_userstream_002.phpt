--TEST--
local user streams must not be able to open() url's
--INI--
allow_url_fopen=1
allow_url_include=0
--FILE--
<?php
class test {
    public $context;
    private $data = '<?php echo "Hello World\n";?>';
    private $pos;
    private $stream = null;

    function stream_open($path, $mode, $options, &$opened_path)
    {
        if (strpos($path, "test2://") === 0) {
            $this->stream = fopen("test1://".substr($path, 8), $mode);
            return !empty($this->stream);
        }
        if (strchr($mode, 'a'))
            $this->pos = strlen($this->data);
        else
            $this->pos = 0;

        return true;
    }

    function stream_read($count)
    {
        if (!empty($this->stream)) {
            return fread($this->stream, $count);
        }
        $ret = substr($this->data, $this->pos, $count);
        $this->pos += strlen($ret);
        return $ret;
    }

    function stream_tell()
    {
        if (!empty($this->stream)) {
            return ftell($this->stream);
        }
        return $this->pos;
    }

    function stream_eof()
    {
        if (!empty($this->stream)) {
            return feof($this->stream);
        }
        return $this->pos >= strlen($this->data);
    }

    function stream_seek($offset, $whence)
    {
        if (!empty($this->stream)) {
            return fseek($this->stream, $offset, $whence);
        }
        switch($whence) {
            case SEEK_SET:
                if ($offset < $this->data && $offset >= 0) {
                    $this->pos = $offset;
                    return true;
                } else {
                    return false;
                }
                break;
            case SEEK_CUR:
                if ($offset >= 0) {
                    $this->pos += $offset;
                    return true;
                } else {
                    return false;
                }
                break;
            case SEEK_END:
                if (strlen($this->data) + $offset >= 0) {
                    $this->pos = strlen($this->data) + $offset;
                    return true;
                } else {
                    return false;
                }
                break;
            default:
                return false;
        }
    }

}

stream_register_wrapper("test1", "test", STREAM_IS_URL);
stream_register_wrapper("test2", "test");
echo @file_get_contents("test1://hello"),"\n";
@include "test1://hello";
echo @file_get_contents("test2://hello"),"\n";
include "test2://hello";
?>
--EXPECTF--
<?php echo "Hello World\n";?>
<?php echo "Hello World\n";?>

Warning: fopen(): test1:// wrapper is disabled in the server configuration by allow_url_include=0 in %sinclude_userstream_002.php on line 11

Warning: fopen(test1://hello): Failed to open stream: no suitable wrapper could be found in %sinclude_userstream_002.php on line 11

Warning: include(test2://hello): Failed to open stream: "test::stream_open" call failed in %sinclude_userstream_002.php on line 90

Warning: include(): Failed opening 'test2://hello' for inclusion (include_path='%s') in %sinclude_userstream_002.php on line 90

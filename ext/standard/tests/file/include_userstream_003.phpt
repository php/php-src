--TEST--
allow_url_fopen disabled
--INI--
allow_url_fopen=0
allow_url_include=1
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
            $this->po = 0;

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
echo file_get_contents("test1://hello"),"\n";
include "test1://hello";
echo file_get_contents("test2://hello"),"\n";
include "test2://hello";
?>
--EXPECTF--
Deprecated: Directive 'allow_url_include' is deprecated in Unknown on line 0

Warning: file_get_contents(): test1:// wrapper is disabled in the server configuration by allow_url_fopen=0 in %s on line %d

Warning: file_get_contents(test1://hello): Failed to open stream: no suitable wrapper could be found in %s on line %d


Warning: include(): test1:// wrapper is disabled in the server configuration by allow_url_fopen=0 in %s on line %d

Warning: include(test1://hello): Failed to open stream: no suitable wrapper could be found in %s on line %d

Warning: include(): Failed opening 'test1://hello' for inclusion (include_path='%s') in %s on line %d

Warning: fopen(): test1:// wrapper is disabled in the server configuration by allow_url_fopen=0 in %s on line %d

Warning: fopen(test1://hello): Failed to open stream: no suitable wrapper could be found in %s on line %d

Warning: file_get_contents(test2://hello): Failed to open stream: "test::stream_open" call failed in %s on line %d


Warning: fopen(): test1:// wrapper is disabled in the server configuration by allow_url_fopen=0 in %s on line %d

Warning: fopen(test1://hello): Failed to open stream: no suitable wrapper could be found in %s on line %d

Warning: include(test2://hello): Failed to open stream: "test::stream_open" call failed in %s on line %d

Warning: include(): Failed opening 'test2://hello' for inclusion (include_path='%s') in %s on line %d

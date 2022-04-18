--TEST--
Bug #27508 (userspace wrappers have bogus eof indicator)
--FILE--
<?php
class FileStream {
    public $fp;
    public $context;

    function stream_open($path, $mode, $options, &$opened_path)
    {
        $url = urldecode(substr($path, 9));
        $this->fp = fopen($url, $mode);

        return true;
    }

    function stream_read($count)
    {
        return fread($this->fp, $count);
    }

    function stream_write($data)
    {
        return fwrite($this->fp, $data);
    }

    function stream_tell()
    {
        return ftell($this->fp);
    }

    function stream_eof()
    {
        if (!$this->fp) {
            return true;
        }
        return feof($this->fp);
    }

    function stream_seek($offset, $whence)
    {
        return fseek($this->fp, $offset, $whence) == 0 ? true : false;
    }
}

stream_wrapper_register("myFile", "FileStream")
    or die("Failed to register protocol");

$tmp_dir = __DIR__;
$tn = tempnam($tmp_dir, 'foo');
if (!$tn) {
  die("tempnam failed");
}

$fp = fopen("myFile://" . urlencode($tn), "w+");
if (!$fp) {
  die("fopen failed");
}

fwrite($fp, "line1\n");
fwrite($fp, "line2\n");
fwrite($fp, "line3\n");

debug_zval_dump(feof($fp));
rewind($fp);
echo ftell($fp) . "\n";
debug_zval_dump(feof($fp));
while ($fp && !feof($fp)) {
    echo fgets($fp);
}
fclose($fp);

unlink($tn);
?>
--EXPECT--
bool(false)
0
bool(false)
line1
line2
line3

<?php

// Copied from ext/sqlite3/tests/stream_test.inc

class PDOSQLite_Test_Stream
{
    public $context;
    private $position;
    public static $string_length = 10;
    public static $string = "abcdefg\0hi";

    public function stream_open($path, $mode, $options, &$opened_path)
    {
        $this->position = 0;
        return true;
    }

    public function stream_read($count)
    {
        $ret = substr(self::$string, $this->position, $count);
        $this->position += strlen($ret);
        return $ret;
    }

    public function stream_write($data)
    {
        return 0;
    }

    public function stream_stat()
    {
        return array('size' => self::$string_length);
    }

    public function stream_tell()
    {
        return $this->position;
    }

    public function stream_eof()
    {
        return ($this->position >= self::$string_length);
    }
}

stream_wrapper_register('pdosqliteBlobTest', PDOSQLite_Test_Stream::class) or die("Unable to register pdosqliteBlobTest stream");

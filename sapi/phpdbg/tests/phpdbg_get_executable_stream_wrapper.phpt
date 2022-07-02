--TEST--
Getting executable lines from custom wrappers
--PHPDBG--
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> array(1) {
  [5]=>
  int(0)
}
[Script ended normally]
prompt>
--FILE--
<?php

/**
 * This example demonstrates how phpdbg_get_executable() behaves differently
 * when passed the 'files' option vs without, in the face of some mild abuse
 * of stream wrappers.
 */

/**
 * First, we define a stream wrapper that simply maps to a real file on disk.
 */
final class StreamWrapper
{
    public $context;
    public function stream_open(
        string $path,
        string $mode,
        int $options = 0,
        string &$openedPath = null
    ) : bool {
        if ($mode[0] !== 'r') {
            return false;
        }

        list($scheme, $path) = explode('://', $path, 2);

        $stream = \fopen($path, $mode);

        if ($stream === false) {
            return false;
        }

        $this->stream = $stream;

        /**
         * The $openedPath reference variable is assigned, indicating the
         * *actual* path that was opened. This affects the behaviour of
         * constants like __FILE__.
         */
        $openedPath = \realpath($path);

        return true;
    }

    public function stream_read(int $count) : string { return \fread($this->stream, $count); }
    public function stream_close() : bool { return \fclose($this->stream); }
    public function stream_eof() : bool { return \feof($this->stream); }
    public function stream_stat() { return \fstat($this->stream); }
    public function stream_set_option($option, $arg1, $arg2) { return false; }

    private $stream = false;
}

stream_wrapper_register('wrapper', StreamWrapper::class);

/**
 * Next, we include a PHP file that contains executable lines, via the stream
 * wrapper.
 */
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'phpdbg_get_executable_stream_wrapper.inc';
require 'wrapper://' . $filename;

/**
 * If we call phpdbg_get_executable() and pass no options, the realpath of the
 * included file is present in the array, but indicates no executable lines.
 */
$x = phpdbg_get_executable();

// We expect [5 => 0], but got an empty array ...
var_dump($x[$filename]);

?>

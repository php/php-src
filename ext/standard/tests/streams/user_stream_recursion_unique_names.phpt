--TEST--
User stream wrapper recursion is bounded even when each nested open uses a unique filename
--FILE--
<?php
class RecWrapper
{
    public $context;
    public static $depth = 0;
    public function stream_open($path, $mode, $options, &$opened_path)
    {
        self::$depth++;
        if (self::$depth >= 100000) {
            return true;
        }
        @fopen('rec://' . self::$depth, 'r');
        return true;
    }
}
stream_wrapper_register('rec', 'RecWrapper');
@fopen('rec://start', 'r');
var_dump(RecWrapper::$depth < 100000);
echo "OK\n";
--EXPECT--
bool(true)
OK

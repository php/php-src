--TEST--
stream_set_chunk_size basic tests
--FILE--
<?php
class test_wrapper {
    function stream_open($path, $mode, $openedpath) {
        return true;
    }
    function stream_eof() {
        return false;
    }
    function stream_read($count) {
        echo "read with size: ", $count, "\n";
        return str_repeat('a', $count);
    }
    function stream_write($data) {
        echo "write with size: ", strlen($data), "\n";
        return strlen($data);
    }
    function stream_set_option($option, $arg1, $arg2) {
        echo "option: ", $option, ", ", $arg1, ", ", $arg2, "\n";
        return false;
    }
}

var_dump(stream_wrapper_register('test', 'test_wrapper'));

$f = fopen("test://foo","r");

/* when the chunk size is 1, the read buffer is skipped, but the
 * the writes are made in chunks of size 1 (business as usual)
 * This should probably be revisited */
echo "should return previous chunk size (8192)\n";
var_dump(stream_set_chunk_size($f, 1));
echo "should be read without buffer (\$count == 10000)\n";
var_dump(strlen(fread($f, 10000)));
echo "should have no effect on writes\n";
var_dump(fwrite($f, str_repeat('b', 3)));

echo "should return previous chunk size (1)\n";
var_dump(stream_set_chunk_size($f, 100));
echo "should elicit one read of size 100 (chunk size)\n";
var_dump(strlen(fread($f, 250)));
echo "should elicit one read of size 100 (chunk size)\n";
var_dump(strlen(fread($f, 50)));
echo "should elicit no read because there is sufficient cached data\n";
var_dump(strlen(fread($f, 50)));
echo "should have no effect on writes\n";
var_dump(strlen(fwrite($f, str_repeat('b', 250))));

echo "\nerror conditions\n";
try {
    stream_set_chunk_size($f, 0);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    stream_set_chunk_size($f, -1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
bool(true)
should return previous chunk size (8192)
int(8192)
should be read without buffer ($count == 10000)
read with size: 10000
int(10000)
should have no effect on writes
write with size: 3
int(3)
should return previous chunk size (1)
int(1)
should elicit one read of size 100 (chunk size)
read with size: 100
int(100)
should elicit one read of size 100 (chunk size)
read with size: 100
int(50)
should elicit no read because there is sufficient cached data
int(50)
should have no effect on writes
write with size: 250
int(3)

error conditions
stream_set_chunk_size(): Argument #2 ($size) must be greater than 0
stream_set_chunk_size(): Argument #2 ($size) must be greater than 0

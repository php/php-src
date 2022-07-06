--TEST--
User-space streams: stream_truncate()
--FILE--
<?php
class test_wrapper_base {
    public $mode;
    function stream_open($path, $mode, $openedpath) {
        return true;
    }
    function stream_eof() {
        return false;
    }
}
class test_wrapper extends test_wrapper_base {
    function stream_truncate($new_size) {
        echo "truncation with new_size=$new_size\n";
        return true;
    }
}
class test_wrapper_bad extends test_wrapper_base {
    function stream_truncate($new_size) {
        echo "truncation with new_size=$new_size\n";
        return "kkk";
    }
}
function test($name, $fd, $dest_size) {
    echo "------ $name: -------\n";
    var_dump(ftruncate($fd, $dest_size));
}
var_dump(stream_wrapper_register('test', 'test_wrapper'));
var_dump(stream_wrapper_register('test2', 'test_wrapper_base'));
var_dump(stream_wrapper_register('test3', 'test_wrapper_bad'));

$fd = fopen("test://foo","r");
$fd2 = fopen("test2://foo","r");
$fd3 = fopen("test3://foo","r");

test("stream_truncate not implemented", $fd2, 0);
test("stream_truncate size 0", $fd, 0);
test("stream_truncate size 10", $fd, 10);
try {
    test("stream_truncate negative size", $fd, -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
test("stream_truncate bad return", $fd3, 0);
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
------ stream_truncate not implemented: -------

Warning: ftruncate(): Can't truncate this stream! in %s on line %d
bool(false)
------ stream_truncate size 0: -------
truncation with new_size=0
bool(true)
------ stream_truncate size 10: -------
truncation with new_size=10
bool(true)
------ stream_truncate negative size: -------
ftruncate(): Argument #2 ($size) must be greater than or equal to 0
------ stream_truncate bad return: -------
truncation with new_size=0

Warning: ftruncate(): test_wrapper_bad::stream_truncate did not return a boolean! in %s on line %d
bool(false)

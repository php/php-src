--TEST--
User-space streams: stream_set_option()
--FILE--
<?php
class test_wrapper_base {
    public $return_value;
    public $expected_option;
    public $expected_value;
    function stream_open($path, $mode, $openedpath) {
        return true;
    }
    function stream_eof() {
        return false;
    }
}
class test_wrapper extends test_wrapper_base {
    function stream_set_option($option, $value, $ptrparam) {
        echo "value:\n";
        var_dump($value);
        echo "ptrparam:\n";
        var_dump($ptrparam);
        echo "\$option === $option === " . $this->expected_option . ":\n";
        var_dump($option === $this->expected_option);
        echo "\$value === $value === " . $this->expected_value. ":\n";
        var_dump($value === $this->expected_value);
        return $this->return_value;
    }
}

function test($name, $fd, $return_value, $func, $args, $expected_option, $expected_value) {
    echo "\n------ $name: -------\n";
    $data = stream_get_meta_data($fd);
    $data['wrapper_data']->return_value = $return_value;
    $data['wrapper_data']->expected_option = $expected_option;
    $data['wrapper_data']->expected_value = $expected_value;
    var_dump(call_user_func_array($func, $args));
}

var_dump(stream_wrapper_register('test', 'test_wrapper'));
var_dump(stream_wrapper_register('test2', 'test_wrapper_base'));

$fd = fopen("test://foo","r");
$fd2 = fopen("test2://foo","r");

test("stream_set_blocking - 1", $fd, true, "stream_set_blocking", array($fd,0), STREAM_OPTION_BLOCKING, 0);
test("stream_set_blocking - 2", $fd, false, "stream_set_blocking", array($fd,1), STREAM_OPTION_BLOCKING, 1);
test("stream_set_blocking - 3", $fd, "foo", "stream_set_blocking", array($fd,0), STREAM_OPTION_BLOCKING, 0);
test("stream_set_blocking - 4", $fd2, true, "stream_set_blocking", array($fd2,1), STREAM_OPTION_BLOCKING, 1);

test("stream_set_write_buffer - 1", $fd, true, "stream_set_write_buffer", array($fd,0), STREAM_OPTION_WRITE_BUFFER, STREAM_BUFFER_NONE);
test("stream_set_write_buffer - 2", $fd, true, "stream_set_write_buffer", array($fd,4096), STREAM_OPTION_WRITE_BUFFER, STREAM_BUFFER_FULL);
test("stream_set_write_buffer - 3", $fd, false, "stream_set_write_buffer", array($fd,8192), STREAM_OPTION_WRITE_BUFFER, STREAM_BUFFER_FULL);

test("stream_set_timeout - 1", $fd, true, "stream_set_timeout", array($fd,10,11), STREAM_OPTION_READ_TIMEOUT, 10);
test("stream_set_timeout - 2", $fd, false, "stream_set_timeout", array($fd,11,12), STREAM_OPTION_READ_TIMEOUT, 11);

?>
--EXPECTF--
bool(true)
bool(true)

------ stream_set_blocking - 1: -------
value:
int(0)
ptrparam:
NULL
$option === 1 === 1:
bool(true)
$value === 0 === 0:
bool(true)
bool(true)

------ stream_set_blocking - 2: -------
value:
int(1)
ptrparam:
NULL
$option === 1 === 1:
bool(true)
$value === 1 === 1:
bool(true)
bool(false)

------ stream_set_blocking - 3: -------
value:
int(0)
ptrparam:
NULL
$option === 1 === 1:
bool(true)
$value === 0 === 0:
bool(true)
bool(true)

------ stream_set_blocking - 4: -------

Warning: stream_set_blocking(): test_wrapper_base::stream_set_option is not implemented! in %s
bool(false)

------ stream_set_write_buffer - 1: -------
value:
int(0)
ptrparam:
int(%d)
$option === 3 === 3:
bool(true)
$value === 0 === 0:
bool(true)
int(0)

------ stream_set_write_buffer - 2: -------
value:
int(2)
ptrparam:
int(4096)
$option === 3 === 3:
bool(true)
$value === 2 === 2:
bool(true)
int(0)

------ stream_set_write_buffer - 3: -------
value:
int(2)
ptrparam:
int(8192)
$option === 3 === 3:
bool(true)
$value === 2 === 2:
bool(true)
int(-1)

------ stream_set_timeout - 1: -------
value:
int(10)
ptrparam:
int(11)
$option === 4 === 4:
bool(true)
$value === 10 === 10:
bool(true)
bool(true)

------ stream_set_timeout - 2: -------
value:
int(11)
ptrparam:
int(12)
$option === 4 === 4:
bool(true)
$value === 11 === 11:
bool(true)
bool(false)

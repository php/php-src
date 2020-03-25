--TEST--
User-space streams: stream_cast()
--FILE--
<?php
class test_wrapper_base {
    public $return_value;
    function stream_open($path, $mode, $openedpath) {
        return true;
    }
    function stream_eof() {
        return false;
    }
}
class test_wrapper extends test_wrapper_base {
    function stream_cast($castas) {
        return $this->return_value;
    }
}
function test($name, $fd, $return_value) {
    echo "\n------ $name: -------\n";
    $data = stream_get_meta_data($fd);
    $data['wrapper_data']->return_value = $return_value;
    $r = array($fd);
    $w = $e = null;
    try {
        var_dump(stream_select($r, $w, $e, 0) !== false);
    } catch (TypeError|ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

var_dump(stream_wrapper_register('test', 'test_wrapper'));
var_dump(stream_wrapper_register('test2', 'test_wrapper_base'));

$fd = fopen("test://foo","r");
$fd2 = fopen("test2://foo","r");

test("valid stream", $fd, STDIN);
test("stream_cast not implemented", $fd2, null);
test("return value is false", $fd, false);
test("return value not a stream resource", $fd, "foo");
test("return value is stream itself", $fd, $fd);
test("return value cannot be casted", $fd, $fd2);

?>
--EXPECTF--
bool(true)
bool(true)

------ valid stream: -------
bool(true)

------ stream_cast not implemented: -------

Warning: stream_select(): test_wrapper_base::stream_cast is not implemented! in %s

Warning: stream_select(): Cannot represent a stream of type user-space as a select()able descriptor in %s
No stream arrays were passed

------ return value is false: -------

Warning: stream_select(): Cannot represent a stream of type user-space as a select()able descriptor in %s
No stream arrays were passed

------ return value not a stream resource: -------

Warning: stream_select(): test_wrapper::stream_cast must return a stream resource in %s

Warning: stream_select(): Cannot represent a stream of type user-space as a select()able descriptor in %s
No stream arrays were passed

------ return value is stream itself: -------

Warning: stream_select(): test_wrapper::stream_cast must not return itself in %s

Warning: stream_select(): Cannot represent a stream of type user-space as a select()able descriptor in %s
No stream arrays were passed

------ return value cannot be casted: -------

Warning: stream_select(): test_wrapper_base::stream_cast is not implemented! in %s

Warning: stream_select(): Cannot represent a stream of type user-space as a select()able descriptor in %s

Warning: stream_select(): Cannot represent a stream of type user-space as a select()able descriptor in %s
No stream arrays were passed

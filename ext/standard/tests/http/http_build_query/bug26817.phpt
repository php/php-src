--TEST--
Bug #26817 (http_build_query() did not handle private & protected object properties)
--FILE--
<?php
class test {
    protected $foo;
    private $bar;
    public $test;

    function foo()
    {
        $this->bar = 'meuh';
        $this->foo = 'lala';
        $this->test = 'test';

        var_dump(http_build_query($this));
    }
}

$obj = new test();
$obj->foo();
var_dump(http_build_query($obj));
?>
--EXPECTF--
Deprecated: http_build_query(): Passing an object for argument #1 $data to http_build_query() is deprecated, call get_object_vars() first instead in %s on line %d
string(27) "foo=lala&bar=meuh&test=test"

Deprecated: http_build_query(): Passing an object for argument #1 $data to http_build_query() is deprecated, call get_object_vars() first instead in %s on line %d
string(9) "test=test"

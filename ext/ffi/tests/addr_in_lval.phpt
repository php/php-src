--TEST--
Assignment to CDATA though FFI::addr() trick
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$f = FFI::cdef("typedef struct { char *bar; } other;");
class Container {
    public $data;
    function __construct($f) { $this->data = $f->new("other"); }
    function &getBar() { return $this->data->bar; } // return by ref to get CData instead of null
}
$container = new Container($f);
$data = $f->new("char[2]");
$data[0] = "1";
FFI::addr($container->getBar())[0] = $f->cast("char*", $data); // directly write it
var_dump($container);
?>
--EXPECT--
object(Container)#2 (1) {
  ["data"]=>
  object(FFI\CData:struct <anonymous>)#3 (1) {
    ["bar"]=>
    object(FFI\CData:char*)#6 (1) {
      [0]=>
      string(1) "1"
    }
  }
}

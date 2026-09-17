--TEST--
Using "_" as a function name is NOT deprecated
--SKIPIF--
<?php
if (extension_loaded('gettext')) {
    die("skip gettext extension defines the _ function");
}
?>
--FILE--
<?php

namespace Foo\Bar {
    function _() {}
}

namespace {
    function _() {}
    echo "OK";
}

?>
--EXPECT--
OK

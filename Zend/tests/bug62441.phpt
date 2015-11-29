--TEST--
Bug #62441: Incorrect strong typing in namespaced child classes
--FILE--
<?php
namespace {
    interface Iface {
        function method(stdClass $o);
    }
}
namespace ns {
    class Foo implements \Iface {
        function method(stdClass $o) { }
    }
    
    (new Foo)->method(new \stdClass);
}
?>
--EXPECTF--
Fatal error: Declaration of ns\Foo::method(ns\stdClass $o) must be compatible with Iface::method(stdClass $o) in %s on line %d


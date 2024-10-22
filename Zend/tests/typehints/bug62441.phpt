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
Fatal error: Could not check compatibility between ns\Foo::method(ns\stdClass $o) and Iface::method(stdClass $o), because class ns\stdClass is not available in %s on line %d

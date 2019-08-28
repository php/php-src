--TEST--
Bug #62441: Incorrect strong typing in namespaced child classes
--FILE--
<?php
namespace {
    interface Iface {
        function method(StdClass $o);
    }
}
namespace ns {
    class Foo implements \Iface {
        function method(StdClass $o) { }
    }

    (new Foo)->method(new \StdClass);
}
?>
--EXPECTF--
Fatal error: Could not check compatibility between ns\Foo::method(ns\StdClass $o) and Iface::method(StdClass $o), because class ns\StdClass is not available in %s on line %d

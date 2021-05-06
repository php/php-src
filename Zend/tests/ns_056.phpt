--TEST--
056: type-hint compatibility in namespaces
--FILE--
<?php
namespace test\ns1;
use \SplObserver;

class Foo implements SplObserver {
    function update(\SplSubject $x): void {
        echo "ok\n";
    }
}

class Bar implements \SplSubject {
    function attach(SplObserver $x): void {
        echo "ok\n";
    }
    function notify(): void {
    }
    function detach(SplObserver $x): void {
    }
}
$foo = new Foo();
$bar = new Bar();
$bar->attach($foo);
$foo->update($bar);
?>
--EXPECT--
ok
ok

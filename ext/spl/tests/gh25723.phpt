--TEST--
GH-25723: spl_autoload_unregister('spl_autoload_call') UAF/double-free on destructor re-entry
--FILE--
<?php

error_reporting(E_ALL & ~E_DEPRECATED);

// 1. Destructor unregisters itself during spl_autoload_unregister('spl_autoload_call')
class SelfUnregister {
    public function autoload($c) {}
    public function __destruct() {
        var_dump(spl_autoload_unregister([$this, 'autoload']));
    }
}
$a = new SelfUnregister();
spl_autoload_register([$a, 'autoload']);
unset($a);
var_dump(spl_autoload_unregister('spl_autoload_call'));
$t = str_repeat('x', 15);
echo "case1 ok\n";

// 2. Destructor of first autoloader unregisters second autoloader
class OtherLoader {
    public function autoload($c) {}
}
class UnregisterOther {
    public $other;
    public function autoload($c) {}
    public function __destruct() {
        var_dump(spl_autoload_unregister([$this->other, 'autoload']));
    }
}
$b = new OtherLoader();
$a = new UnregisterOther();
$a->other = $b;
spl_autoload_register([$a, 'autoload']);
spl_autoload_register([$b, 'autoload']);
unset($a, $b);
var_dump(spl_autoload_unregister('spl_autoload_call'));
$x = str_repeat('x', 7);
$y = str_repeat('y', 7);
echo "case2 ok\n";

// 3. Destructor re-enters spl_autoload_unregister('spl_autoload_call') and registers a new loader
class ReplacementLoader {
    public function autoload($c) {}
}
class ReenterAndRegister {
    public function autoload($c) {}
    public function __destruct() {
        var_dump(spl_autoload_unregister('spl_autoload_call'));
        spl_autoload_register([new ReplacementLoader(), 'autoload']);
    }
}
$a = new ReenterAndRegister();
spl_autoload_register([$a, 'autoload']);
unset($a);
var_dump(spl_autoload_unregister('spl_autoload_call'));
var_dump(count(spl_autoload_functions()));
var_dump(spl_autoload_unregister('spl_autoload_call'));
var_dump(count(spl_autoload_functions()));
echo "case3 ok\n";

// 4. Closure capturing $this and __call trampoline autoloader
class ClosureLoader {
    public function makeClosure() {
        return function ($c) {};
    }
    public function __destruct() {
        var_dump(count(spl_autoload_functions()));
        foreach (spl_autoload_functions() as $f) {
            spl_autoload_unregister($f);
        }
    }
}
$c1 = new ClosureLoader();
spl_autoload_register($c1->makeClosure());
unset($c1);
var_dump(spl_autoload_unregister('spl_autoload_call'));

class TrampolineLoader {
    public function __call($name, $args) {}
    public function __destruct() {
        var_dump(spl_autoload_unregister([$this, 'dynamicAutoload']));
    }
}
$c2 = new TrampolineLoader();
spl_autoload_register([$c2, 'dynamicAutoload']);
unset($c2);
var_dump(spl_autoload_unregister('spl_autoload_call'));
echo "case4 ok\n";

// 5. Destructor throwing an exception still cleans remaining entries without leaking
class ThrowingLoader {
    public function autoload($c) {}
    public function __destruct() {
        throw new RuntimeException('boom');
    }
}
spl_autoload_register([new ThrowingLoader(), 'autoload']);
spl_autoload_register([new OtherLoader(), 'autoload']);
try {
    spl_autoload_unregister('spl_autoload_call');
} catch (RuntimeException $e) {
    echo "caught: ", $e->getMessage(), "\n";
}
var_dump(count(spl_autoload_functions()));
echo "case5 ok\n";

// 6. Calling spl_autoload_unregister('spl_autoload_call') inside an active autoloader
class ClearingDuringAutoload {
    public function autoload($c) {
        spl_autoload_unregister('spl_autoload_call');
    }
}
spl_autoload_register([new ClearingDuringAutoload(), 'autoload']);
spl_autoload_register([new OtherLoader(), 'autoload']);
spl_autoload_call('GH25723NonExistentClass');
var_dump(count(spl_autoload_functions()));
echo "case6 ok\n";
?>
--EXPECT--
bool(false)
bool(true)
case1 ok
bool(false)
bool(true)
case2 ok
bool(true)
bool(true)
int(1)
bool(true)
int(0)
case3 ok
int(0)
bool(true)
bool(false)
bool(true)
case4 ok
caught: boom
int(0)
case5 ok
int(0)
case6 ok

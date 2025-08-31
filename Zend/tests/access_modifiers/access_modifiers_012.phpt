--TEST--
Trigger __call() in lieu of non visible methods when called via a callback.
--FILE--
<?php
class C {
    protected function prot() { }
    private function priv() { }
    public function __call($name, $args)    {
        echo "In __call() for method $name()\n";
    }
}

$c = new C;
call_user_func(array($c, 'none'));
call_user_func(array($c, 'prot'));
call_user_func(array($c, 'priv'));
?>
--EXPECT--
In __call() for method none()
In __call() for method prot()
In __call() for method priv()

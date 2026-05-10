--TEST--
Diamond + property hook (get-only): covariant T merges return type as intersection
--FILE--
<?php
interface HasX<out T : object> {
    public T $x { get; }
}
interface IFoo {}
interface IBar {}
class FooBar implements IFoo, IBar {}

interface FlexX extends HasX<IFoo>, HasX<IBar> {}

$r = new ReflectionClass(FlexX::class);
$prop = $r->getProperty('x');
$hooks = $prop->getHooks();
foreach ($hooks as $kind => $hook) {
    if ($hook->hasReturnType()) {
        echo "hook $kind return: ", (string)$hook->getReturnType(), "\n";
    }
}

class C implements FlexX {
    public IFoo&IBar $x { get => new FooBar(); }
}
$c = new C;
var_dump($c->x instanceof IFoo, $c->x instanceof IBar);
?>
--EXPECT--
hook get return: IFoo&IBar
bool(true)
bool(true)

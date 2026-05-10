--TEST--
Diamond + invariant T: interface diamond synthesises use-site-merged abstract contract
--FILE--
<?php
interface IFoo {}
interface IBar {}

interface Tapper<T : object> {
    public function tap(T $object): T;
}

interface FooTapper extends Tapper<IFoo> {}
interface BarTapper extends Tapper<IBar> {}

interface FlexTapper extends FooTapper, BarTapper {}

$r = new ReflectionClass(FlexTapper::class);
$m = $r->getMethod('tap');
echo "param: ", (string)$m->getParameters()[0]->getType(), "\n";
echo "return: ", (string)$m->getReturnType(), "\n";

interface FlexDirect extends Tapper<IFoo>, Tapper<IBar> {}
$rd = new ReflectionClass(FlexDirect::class);
$md = $rd->getMethod('tap');
echo "direct param: ", (string)$md->getParameters()[0]->getType(), "\n";
echo "direct return: ", (string)$md->getReturnType(), "\n";
?>
--EXPECT--
param: IFoo|IBar
return: IFoo&IBar
direct param: IFoo|IBar
direct return: IFoo&IBar

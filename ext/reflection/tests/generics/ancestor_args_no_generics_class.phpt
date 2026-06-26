--TEST--
Reflection: classes with no generic content — empty array when ancestor without args, throw when not an ancestor
--FILE--
<?php
class Plain {}
class WithParent extends Plain {}
interface IPlain {}
class WithIface implements IPlain {}
trait TPlain { public function noop(): void {} }
class WithTrait { use TPlain; }

function show(string $cls): void {
    $rc = new ReflectionClass($cls);

    try {
        $p = $rc->getGenericArgumentsForParentClass();
        echo $cls, ": parent=", json_encode($p);
    } catch (ReflectionException $e) {
        echo $cls, ": parent=throw(", $e->getMessage(), ")";
    }

    try {
        $i = $rc->getGenericArgumentsForParentInterface('IPlain');
        echo " iface=", json_encode($i);
    } catch (ReflectionException $e) {
        echo " iface=throw(", $e->getMessage(), ")";
    }

    try {
        $t = $rc->getGenericArgumentsForUsedTrait('TPlain');
        echo " trait=", json_encode($t);
    } catch (ReflectionException $e) {
        echo " trait=throw(", $e->getMessage(), ")";
    }

    echo "\n";
}

foreach (['Plain', 'WithParent', 'WithIface', 'WithTrait'] as $cls) {
    show($cls);
}
?>
--EXPECT--
Plain: parent=throw(Class Plain has no parent class) iface=throw(IPlain is not an ancestor interface of Plain) trait=throw(TPlain is not a trait used by Plain)
WithParent: parent=[] iface=throw(IPlain is not an ancestor interface of WithParent) trait=throw(TPlain is not a trait used by WithParent)
WithIface: parent=throw(Class WithIface has no parent class) iface=[] trait=throw(TPlain is not a trait used by WithIface)
WithTrait: parent=throw(Class WithTrait has no parent class) iface=throw(IPlain is not an ancestor interface of WithTrait) trait=[]

--TEST--
GH-20112 (Nested load_delayed_classes must not process unrelated outer entries)
--FILE--
<?php
spl_autoload_register(function ($class) {
    if ($class == 'IFace') {
        interface IFace {}
    } elseif ($class == 'IFaceImplementor') {
        class IFaceImplementor implements IFace {}
    } elseif ($class == 'InterfaceOfMainParent') {
        interface InterfaceOfMainParent {
            public function methodForSecondaryLspCheck(): IFace;
        }
    } elseif ($class == 'MainParent') {
        abstract class MainParent implements InterfaceOfMainParent {
            public function methodForSecondaryLspCheck(): IFaceImplementor {}
        }
    } elseif ($class == 'Intermediate') {
        abstract class Intermediate extends MainParent {}
    } elseif ($class == 'Child1') {
        class Child1 extends Intermediate {}
    } elseif ($class == 'Child2') {
        class Child2 extends Intermediate {}
    } elseif ($class == 'EntrypointParent') {
        abstract class EntrypointParent {
            abstract public function methodForLspCheck1(): MainParent;
            abstract public function methodForLspCheck2(): MainParent;
        }
    } elseif ($class == 'Entrypoint') {
        class Entrypoint extends EntrypointParent {
            public function methodForLspCheck1(): Child1 {}
            public function methodForLspCheck2(): Child2 {}
        }
    }
});

class_exists(Entrypoint::class);
echo "OK\n";
?>
--EXPECT--
OK

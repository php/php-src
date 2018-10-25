--TEST--
Rebinding of ::getClosure()s
--FILE--
<?php

use SplDoublyLinkedList as DLL;

function func($arg) { }

class Cls {
    public function method() {}
    public static function staticMethod($arg) {}
}

class ClsChild extends Cls {}

class ClsUnrelated {}

/* Format: [Function, [Obj, Scope]] */
$tests = [
    ['func', [
        [null,         null],
        [new Cls,      null],
        [new Cls,      'Cls'],
        [null,         'Cls'],
        [null,         'stdClass'],
        [new stdClass, null],
    ]],

    ['strlen', [
        [null,         null],
        [new Cls,      null],
        [new Cls,      'Cls'],
        [null,         'Cls'],
        [null,         'stdClass'],
        [new stdClass, null],
    ]],

    [['Cls', 'staticMethod'], [
        [null,   'Cls'],
        [new Cls, null],
        [new Cls, 'Cls'],
        [null,    null],
        [null,    'ClsChild'],
        [null,    'ClsUnrelated'],
    ]],

    [[new Cls, 'method'], [
        [null,             'Cls'],
        [new Cls,          'Cls'],
        [new ClsChild,     'Cls'],
        [new ClsUnrelated, 'Cls'],
        [new Cls,          null],
        [new Cls,          'ClsUnrelated'],
        [new Cls,          'ClsChild'],
    ]],

    [[new DLL, 'count'], [
        [new DLL, DLL::class],
        [new SplStack, DLL::class],
        [new ClsUnrelated, DLL::class],
        [null, null],
        [null, DLL::class],
        [new DLL, null],
        [new DLL, ClsUnrelated::class],
    ]],

    [function() {}, [
        [null,         null],
        [new Cls,      null],
        [new Cls,      'Cls'],
        [null,         'Cls'],
        [null,         'stdClass'],
        [new stdClass, null],
    ]],
];

set_error_handler(function($errno, $errstr) {
    echo "$errstr\n\n";
});

foreach ($tests as list($fn, $bindings)) {
    if (is_array($fn)) {
        $r = new ReflectionMethod($fn[0], $fn[1]);
        $c = $r->getClosure(is_object($fn[0]) ? $fn[0] : null);
        $fnStr = is_object($fn[0]) ? "(new " . get_class($fn[0]) . ")->$fn[1]" : "$fn[0]::$fn[1]";
    } else {
        $c = (new ReflectionFunction($fn))->getClosure();
        $fnStr = $fn;
    }
    if ($fn instanceof Closure) {
        $fnStr = "(function() {})";
    }

    echo "$fnStr()\n" . str_repeat('-', strlen($fnStr) + 2), "\n\n";

    foreach ($bindings as list($obj, $scope)) {
        $objStr = $obj ? "new " . get_class($obj) : "null";
        $scopeStr = $scope ? "$scope::class" : "null";
        echo "bindTo($objStr, $scopeStr):\n";

        $ret = $c->bindTo($obj, $scope);
        if ($ret !== null) {
            echo "Success!\n\n";
        }
    }
}

?>
--EXPECT--
func()
------

bindTo(null, null):
Success!

bindTo(new Cls, null):
Success!

bindTo(new Cls, Cls::class):
Cannot rebind scope of closure created by ReflectionFunctionAbstract::getClosure()

bindTo(null, Cls::class):
Cannot rebind scope of closure created by ReflectionFunctionAbstract::getClosure()

bindTo(null, stdClass::class):
Cannot bind closure to scope of internal class stdClass

bindTo(new stdClass, null):
Success!

strlen()
--------

bindTo(null, null):
Success!

bindTo(new Cls, null):
Success!

bindTo(new Cls, Cls::class):
Cannot rebind scope of closure created by ReflectionFunctionAbstract::getClosure()

bindTo(null, Cls::class):
Cannot rebind scope of closure created by ReflectionFunctionAbstract::getClosure()

bindTo(null, stdClass::class):
Cannot bind closure to scope of internal class stdClass

bindTo(new stdClass, null):
Success!

Cls::staticMethod()
-------------------

bindTo(null, Cls::class):
Success!

bindTo(new Cls, null):
Cannot bind an instance to a static closure

bindTo(new Cls, Cls::class):
Cannot bind an instance to a static closure

bindTo(null, null):
Cannot rebind scope of closure created by ReflectionFunctionAbstract::getClosure()

bindTo(null, ClsChild::class):
Cannot rebind scope of closure created by ReflectionFunctionAbstract::getClosure()

bindTo(null, ClsUnrelated::class):
Cannot rebind scope of closure created by ReflectionFunctionAbstract::getClosure()

(new Cls)->method()
-------------------

bindTo(null, Cls::class):
Unbinding $this of a method is deprecated

Success!

bindTo(new Cls, Cls::class):
Success!

bindTo(new ClsChild, Cls::class):
Success!

bindTo(new ClsUnrelated, Cls::class):
Cannot bind method Cls::method() to object of class ClsUnrelated

bindTo(new Cls, null):
Cannot rebind scope of closure created by ReflectionFunctionAbstract::getClosure()

bindTo(new Cls, ClsUnrelated::class):
Cannot rebind scope of closure created by ReflectionFunctionAbstract::getClosure()

bindTo(new Cls, ClsChild::class):
Cannot rebind scope of closure created by ReflectionFunctionAbstract::getClosure()

(new SplDoublyLinkedList)->count()
----------------------------------

bindTo(new SplDoublyLinkedList, SplDoublyLinkedList::class):
Success!

bindTo(new SplStack, SplDoublyLinkedList::class):
Success!

bindTo(new ClsUnrelated, SplDoublyLinkedList::class):
Cannot bind method SplDoublyLinkedList::count() to object of class ClsUnrelated

bindTo(null, null):
Cannot unbind $this of internal method

bindTo(null, SplDoublyLinkedList::class):
Cannot unbind $this of internal method

bindTo(new SplDoublyLinkedList, null):
Cannot rebind scope of closure created by ReflectionFunctionAbstract::getClosure()

bindTo(new SplDoublyLinkedList, ClsUnrelated::class):
Cannot rebind scope of closure created by ReflectionFunctionAbstract::getClosure()

(function() {})()
-----------------

bindTo(null, null):
Success!

bindTo(new Cls, null):
Success!

bindTo(new Cls, Cls::class):
Success!

bindTo(null, Cls::class):
Success!

bindTo(null, stdClass::class):
Cannot bind closure to scope of internal class stdClass

bindTo(new stdClass, null):
Success!

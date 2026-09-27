--TEST--
ArrayObject and ArrayIterator separate a shared array before writing to it
--FILE--
<?php
// Every write path separates the storage from the array it was built from

function check(string $label, callable $write, array $classes = ['ArrayIterator', 'ArrayObject']) {
    foreach ($classes as $class) {
        $source = ['a' => 1, 'b' => ['x' => 1], 'c' => 3];
        $object = new $class($source, ArrayObject::ARRAY_AS_PROPS);
        $copy = $object->getArrayCopy();
        $write($object);
        if ($source !== ['a' => 1, 'b' => ['x' => 1], 'c' => 3] || $copy !== $source) {
            echo "$class $label: FAIL\n";
            var_dump($source, $copy);
        }
        echo "$class $label: ", json_encode($object->getArrayCopy()), "\n";
    }
}

check('offsetSet', function ($o) { $o['a'] = 2; });
check('offsetSet()', function ($o) { $o->offsetSet('a', 2); });
check('append', function ($o) { $o[] = 4; });
check('append()', function ($o) { $o->append(4); });
check('offsetUnset', function ($o) { unset($o['a']); });
check('offsetUnset() missing', function ($o) { $o->offsetUnset('z'); });
check('compound assign', function ($o) { $o['a'] += 10; });
check('increment', function ($o) { $o['a']++; });
check('nested write', function ($o) { $o['b']['y'] = 2; });
check('nested append', function ($o) { $o['b'][] = 2; });
check('nested unset', function ($o) { unset($o['b']['x']); });
check('reference', function ($o) { $r = &$o['a']; $r = 5; });
check('property', function ($o) { $o->a = 6; });
check('property compound assign', function ($o) { $o->a .= 'x'; });
check('property nested write', function ($o) { $o->b['z'] = 3; });
check('property reference', function ($o) { $r = &$o->c; $r = 7; });
check('property unset', function ($o) { unset($o->c); });
check('foreach by reference', function ($o) { foreach ($o as &$v) { $v = 8; } });
check('uasort', function ($o) { $o->uasort(fn ($a, $b) => is_array($a) <=> is_array($b)); }, ['ArrayObject']);
check('ksort', function ($o) { $o->ksort(); $o['d'] = 0; }, ['ArrayObject']);
check('exchangeArray', function ($o) { if ($o instanceof ArrayObject) { $o->exchangeArray(['e' => 5]); } else { $o->__construct(['e' => 5]); } });
check('unserialize', function ($o) { $o->__unserialize([0, ['f' => 6], [], null]); });
?>
--EXPECT--
ArrayIterator offsetSet: {"a":2,"b":{"x":1},"c":3}
ArrayObject offsetSet: {"a":2,"b":{"x":1},"c":3}
ArrayIterator offsetSet(): {"a":2,"b":{"x":1},"c":3}
ArrayObject offsetSet(): {"a":2,"b":{"x":1},"c":3}
ArrayIterator append: {"a":1,"b":{"x":1},"c":3,"0":4}
ArrayObject append: {"a":1,"b":{"x":1},"c":3,"0":4}
ArrayIterator append(): {"a":1,"b":{"x":1},"c":3,"0":4}
ArrayObject append(): {"a":1,"b":{"x":1},"c":3,"0":4}
ArrayIterator offsetUnset: {"b":{"x":1},"c":3}
ArrayObject offsetUnset: {"b":{"x":1},"c":3}
ArrayIterator offsetUnset() missing: {"a":1,"b":{"x":1},"c":3}
ArrayObject offsetUnset() missing: {"a":1,"b":{"x":1},"c":3}
ArrayIterator compound assign: {"a":11,"b":{"x":1},"c":3}
ArrayObject compound assign: {"a":11,"b":{"x":1},"c":3}
ArrayIterator increment: {"a":2,"b":{"x":1},"c":3}
ArrayObject increment: {"a":2,"b":{"x":1},"c":3}
ArrayIterator nested write: {"a":1,"b":{"x":1,"y":2},"c":3}
ArrayObject nested write: {"a":1,"b":{"x":1,"y":2},"c":3}
ArrayIterator nested append: {"a":1,"b":{"x":1,"0":2},"c":3}
ArrayObject nested append: {"a":1,"b":{"x":1,"0":2},"c":3}
ArrayIterator nested unset: {"a":1,"b":[],"c":3}
ArrayObject nested unset: {"a":1,"b":[],"c":3}
ArrayIterator reference: {"a":5,"b":{"x":1},"c":3}
ArrayObject reference: {"a":5,"b":{"x":1},"c":3}
ArrayIterator property: {"a":6,"b":{"x":1},"c":3}
ArrayObject property: {"a":6,"b":{"x":1},"c":3}
ArrayIterator property compound assign: {"a":"1x","b":{"x":1},"c":3}
ArrayObject property compound assign: {"a":"1x","b":{"x":1},"c":3}
ArrayIterator property nested write: {"a":1,"b":{"x":1,"z":3},"c":3}
ArrayObject property nested write: {"a":1,"b":{"x":1,"z":3},"c":3}
ArrayIterator property reference: {"a":1,"b":{"x":1},"c":7}
ArrayObject property reference: {"a":1,"b":{"x":1},"c":7}
ArrayIterator property unset: {"a":1,"b":{"x":1}}
ArrayObject property unset: {"a":1,"b":{"x":1}}
ArrayIterator foreach by reference: {"a":8,"b":8,"c":8}
ArrayObject foreach by reference: {"a":8,"b":8,"c":8}
ArrayObject uasort: {"a":1,"c":3,"b":{"x":1}}
ArrayObject ksort: {"a":1,"b":{"x":1},"c":3,"d":0}
ArrayIterator exchangeArray: {"e":5}
ArrayObject exchangeArray: {"e":5}
ArrayIterator unserialize: {"f":6}
ArrayObject unserialize: {"f":6}

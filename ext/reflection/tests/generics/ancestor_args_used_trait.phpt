--TEST--
Reflection: getGenericArgumentsForUsedTrait returns args from use clause; throws when trait not used
--FILE--
<?php
trait Holder<T = mixed> { public T $val; }
trait Pair<K, V> { public K $k; public V $v; }
trait Plain { public int $x; }

class WithArgs { use Holder<string>; }
class NoArgs   { use Holder; }
class NoTrait  {}
class Multi    { use Holder<bool>, Pair<int, float>; }
class Combo    { use Holder<string>, Plain; }

function show(string $cls, string $tr): void {
    try {
        $args = (new ReflectionClass($cls))->getGenericArgumentsForUsedTrait($tr);
    } catch (ReflectionException $e) {
        echo "$cls/$tr: throw ({$e->getMessage()})\n";
        return;
    }
    if (!$args) {
        echo "$cls/$tr: []\n";
        return;
    }
    echo "$cls/$tr: ", implode(",", array_map(fn($t)=>$t->getName(), $args)), "\n";
}

show('WithArgs', 'Holder');
show('NoArgs', 'Holder');
show('NoTrait', 'Holder');
show('Multi', 'Holder');
show('Multi', 'Pair');
show('Combo', 'Holder');
show('Combo', 'Plain');
show('Combo', 'holder'); // case insensitive
?>
--EXPECT--
WithArgs/Holder: string
NoArgs/Holder: []
NoTrait/Holder: throw (Holder is not a trait used by NoTrait)
Multi/Holder: bool
Multi/Pair: int,float
Combo/Holder: string
Combo/Plain: []
Combo/holder: string

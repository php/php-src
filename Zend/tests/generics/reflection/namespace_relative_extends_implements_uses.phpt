--TEST--
Reflection: namespace-relative `extends`/`implements`/`use` keep their generic arguments in the side table
--FILE--
<?php declare(strict_types=1);

namespace NSRelExtImpUse;

class Base<out T> {}
interface Iface<out T> {}
trait Mixin<out T> {}

class FqnExt extends \NSRelExtImpUse\Base<int> {}
class NsExt extends namespace\Base<int> {}
class RelExt extends Base<int> {}

class FqnImpl implements \NSRelExtImpUse\Iface<int> {}
class NsImpl implements namespace\Iface<int> {}
class RelImpl implements Iface<int> {}

class FqnUse { use \NSRelExtImpUse\Mixin<int>; }
class NsUse { use namespace\Mixin<int>; }
class RelUse { use Mixin<int>; }

$singles = [
    'extends FQN' => (new \ReflectionClass(FqnExt::class))->getGenericArgumentsForParentClass(),
    'extends ns'  => (new \ReflectionClass(NsExt::class))->getGenericArgumentsForParentClass(),
    'extends rel' => (new \ReflectionClass(RelExt::class))->getGenericArgumentsForParentClass(),
    'use FQN'     => (new \ReflectionClass(FqnUse::class))->getGenericArgumentsForUsedTrait(Mixin::class),
    'use ns'      => (new \ReflectionClass(NsUse::class))->getGenericArgumentsForUsedTrait(Mixin::class),
    'use rel'     => (new \ReflectionClass(RelUse::class))->getGenericArgumentsForUsedTrait(Mixin::class),
];

foreach ($singles as $label => $args) {
    $rendered = array_map(static fn(\ReflectionNamedType $t): string => $t->getName(), $args);
    printf("%-12s count=%d args=[%s]\n", $label, count($args), implode(', ', $rendered));
}

$plural = [
    'impl FQN'    => (new \ReflectionClass(FqnImpl::class))->getGenericArgumentsForParentInterface(Iface::class),
    'impl ns'     => (new \ReflectionClass(NsImpl::class))->getGenericArgumentsForParentInterface(Iface::class),
    'impl rel'    => (new \ReflectionClass(RelImpl::class))->getGenericArgumentsForParentInterface(Iface::class),
];

foreach ($plural as $label => $bindings) {
    $rendered = array_map(
        static fn(array $b): string => '[' . implode(', ', array_map(
            static fn(\ReflectionNamedType $t): string => $t->getName(),
            $b,
        )) . ']',
        $bindings,
    );
    printf("%-12s bindings=%d sets=[%s]\n", $label, count($bindings), implode(', ', $rendered));
}
?>
--EXPECT--
extends FQN  count=1 args=[int]
extends ns   count=1 args=[int]
extends rel  count=1 args=[int]
use FQN      count=1 args=[int]
use ns       count=1 args=[int]
use rel      count=1 args=[int]
impl FQN     bindings=1 sets=[[int]]
impl ns      bindings=1 sets=[[int]]
impl rel     bindings=1 sets=[[int]]

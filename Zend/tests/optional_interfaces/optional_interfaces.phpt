--TEST--
Optional interfaces work
--FILE--
<?php

interface ExistingInterface {}

class ImplementingOptionalInterface implements ?ExistingInterface {}
class SkippingOptionalInterface implements ?NonExistantInterface {}
class MultipleOptionalsExistingFirst implements ?ExistingInterface, ?NonExistantInterface {}
class MultipleOptionalsExistingLast implements ?NonExistantInterface, ?ExistingInterface {}
class MixedOptionalFirst implements ?NonExistantInterface, ExistingInterface {}
class MixedOptionalLast implements ExistingInterface, ?NonExistantInterface {}

interface ExtendingExistingOptional extends ExistingInterface {}
interface ExtendingNonexistantOptional extends ?NonExistantInterface {}

class ImplementsInheritedExisting implements ExtendingExistingOptional {}
class ImplementsInheritedSkipped implements ExtendingNonexistantOptional {}

$classes = [
    ImplementingOptionalInterface::class,
    SkippingOptionalInterface::class,
    MultipleOptionalsExistingFirst::class,
    MultipleOptionalsExistingLast::class,
    MixedOptionalFirst::class,
    MixedOptionalLast::class,
    ImplementsInheritedExisting::class,
    ImplementsInheritedSkipped::class,
];

foreach ($classes as $class) {
    echo "$class\n";
    echo '    class implements '.implode(', ', class_implements($class))."\n";
    echo '    object implements '.implode(', ', class_implements(new $class))."\n";
}

$interfaces = [
    ExtendingExistingOptional::class,
    ExtendingNonexistantOptional::class,
];

foreach ($interfaces as $interface) {
    echo "$interface\n";
    echo '    interface extends '.implode(', ', class_implements($class))."\n";
}

?>
--EXPECT--
ImplementingOptionalInterface
    class implements ExistingInterface
    object implements ExistingInterface
SkippingOptionalInterface
    class implements
    object implements
MultipleOptionalsExistingFirst
    class implements ExistingInterface
    object implements ExistingInterface
MultipleOptionalsExistingLast
    class implements ExistingInterface
    object implements ExistingInterface
MixedOptionalFirst
    class implements ExistingInterface
    object implements ExistingInterface
MixedOptionalLast
    class implements ExistingInterface
    object implements ExistingInterface
ImplementsInheritedExisting
    class implements ExtendingExistingOptional, ExistingInterface
    object implements ExtendingExistingOptional, ExistingInterface
ImplementsInheritedSkipped
    class implements ExtendingNonexistantOptional
    object implements ExtendingNonexistantOptional
ExtendingExistingOptional
    interface extends ExtendingNonexistantOptional
ExtendingNonexistantOptional
    interface extends ExtendingNonexistantOptional

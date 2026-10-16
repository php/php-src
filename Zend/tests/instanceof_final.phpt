--TEST--
instanceof against a final class or an enum
--FILE--
<?php

final class FinalClass {}
class NonFinal {}
class Child extends NonFinal {}

interface Iface {}
final class FinalImpl implements Iface {}

enum UserEnum { case A; }
enum Backed: string { case A = 'a'; }

readonly final class ReadonlyFinal {}

class Base {}
final class Leaf extends Base {
    public function test(): void {
        var_dump($this instanceof self);
        var_dump($this instanceof parent);
        var_dump($this instanceof static);
        var_dump($this instanceof Leaf);
        var_dump($this instanceof Base);
    }
}

// Final class: hit and misses.
var_dump(new FinalClass() instanceof FinalClass);
var_dump(new NonFinal() instanceof FinalClass);
var_dump(new Child() instanceof FinalClass);

// A final class implementing an interface still matches the interface.
var_dump(new FinalImpl() instanceof Iface);
var_dump(new NonFinal() instanceof Iface);

// Userland enum: hit and miss against the enum itself.
var_dump(UserEnum::A instanceof UserEnum);
var_dump(new NonFinal() instanceof UserEnum);

// A userland enum implements UnitEnum, a backed enum also implements BackedEnum.
var_dump(UserEnum::A instanceof UnitEnum);
var_dump(UserEnum::A instanceof BackedEnum);
var_dump(Backed::A instanceof UnitEnum);
var_dump(Backed::A instanceof BackedEnum);
var_dump(new NonFinal() instanceof UnitEnum);
var_dump(new NonFinal() instanceof BackedEnum);

// Internal enum (does not carry ZEND_ACC_FINAL, only ZEND_ACC_ENUM).
var_dump(RoundingMode::HalfAwayFromZero instanceof RoundingMode);
var_dump(new NonFinal() instanceof RoundingMode);

// Readonly final class.
var_dump(new ReadonlyFinal() instanceof ReadonlyFinal);
var_dump(new NonFinal() instanceof ReadonlyFinal);

// Non-object left operand.
var_dump(null instanceof FinalClass);
var_dump(1 instanceof FinalClass);
var_dump("x" instanceof FinalClass);
var_dump([] instanceof FinalClass);

// Undefined right operand is not autoloaded and yields false.
var_dump(new NonFinal() instanceof UndefinedClass);

// Right operand as a variable.
$name = FinalClass::class;
var_dump(new FinalClass() instanceof $name);
$name = NonFinal::class;
var_dump(new FinalClass() instanceof $name);

// self, parent and static.
(new Leaf())->test();

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

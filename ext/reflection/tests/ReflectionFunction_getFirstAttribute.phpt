--TEST--
ReflectionFunction::getFirstAttribute()
--FILE--
<?php
#[Attribute(Attribute::TARGET_FUNCTION | Attribute::IS_REPEATABLE)]
class AnAttribute {
    public function __construct(public readonly string $value) {}
}

#[Attribute(Attribute::TARGET_FUNCTION)]
class AnotherAttribute extends AnAttribute {
    public function __construct(public readonly string $value) {}
}

#[AnotherAttribute('first'), AnAttribute('second'), AnAttribute('third')]
function foo() {}

$function = new ReflectionFunction('foo');
$attribute = $function->getFirstAttribute(AnAttribute::class);
var_dump($attribute);
var_dump($attribute->newInstance()->value);
var_dump($function->getFirstAttribute(AnAttribute::class, ReflectionAttribute::IS_INSTANCEOF)->newInstance()->value);

$function = new ReflectionFunction('foo');
var_dump($function->getFirstAttribute(Stringable::class));
?>
--EXPECT--
object(ReflectionAttribute)#2 (0) {
}
string(6) "second"
string(5) "first"
NULL

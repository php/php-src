--TEST--
Bug GH-8421: Attributes that target functions are not valid for anonymous functions defined within a method
--FILE--
<?php
#[Attribute(Attribute::TARGET_FUNCTION)]
class FunctionAttribute
{
    public int $number = 1;
}

$globalClosure = #[FunctionAttribute]
fn() => true;
$globalStaticClosure = #[FunctionAttribute]
static fn() => true;

class ClosureHolder
{
    public function getClosureDefinedInScope(): Closure
    {
        return #[FunctionAttribute]
        fn() => true;
    }

    public function getStaticClosureDefinedInScope(): Closure
    {
        return #[FunctionAttribute]
        static fn() => true;
    }

    public static function getClosureDefinedInScopeStatically(): Closure
    {
        return #[FunctionAttribute]
        fn() => true;
    }

    public static function getStaticClosureDefinedInScopeStatically(): Closure
    {
        return #[FunctionAttribute]
        static fn() => true;
    }
}

var_dump((new ReflectionFunction($globalClosure))->getAttributes(FunctionAttribute::class)[0]->newInstance()->number);
var_dump((new ReflectionFunction($globalStaticClosure))->getAttributes(FunctionAttribute::class)[0]->newInstance()->number);
var_dump((new ReflectionFunction(ClosureHolder::getClosureDefinedInScopeStatically()))->getAttributes(FunctionAttribute::class)[0]->newInstance()->number);
var_dump((new ReflectionFunction(ClosureHolder::getStaticClosureDefinedInScopeStatically()))->getAttributes(FunctionAttribute::class)[0]->newInstance()->number);

$holder = new ClosureHolder;

var_dump((new ReflectionFunction($holder->getClosureDefinedInScope()))->getAttributes(FunctionAttribute::class)[0]->newInstance()->number);
var_dump((new ReflectionFunction($holder->getStaticClosureDefinedInScope()))->getAttributes(FunctionAttribute::class)[0]->newInstance()->number);
?>
--EXPECT--
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)

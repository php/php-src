--TEST--
PFA in constexpr: sites
--FILE--
<?php

#[Attribute]
class A {
    function __construct(public mixed $fn) {}
}

#[A(strlen(?))]
class C {
    const CC = strlen(?);

    #[A(strlen(?))]
    function f($arg = strlen(?)) {
        return $arg;
    }
}

const CC = strlen(?);

echo "# Class const\n";
var_dump((C::CC)('hello'));
echo "# Const\n";
var_dump((CC)('hello'));
echo "# Class attr\n";
var_dump((new ReflectionClass(C::class)->getAttributes(A::class)[0]->newInstance()->fn)('hello'));
echo "# Method attr\n";
var_dump((new ReflectionMethod(C::class, 'f')->getAttributes(A::class)[0]->newInstance()->fn)('hello'));
echo "# Method default value\n";
var_dump((new C()->f())('hello'));

?>
--EXPECT--
# Class const
int(5)
# Const
int(5)
# Class attr
int(5)
# Method attr
int(5)
# Method default value
int(5)

--TEST--
Partial function application on methods named assert() must invoke the method, while real global assert(?) keeps working
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

class Validator {
    public function assert(bool $cond, string $label = 'default'): string {
        return "INSTANCE:$label:" . ($cond ? 'true' : 'false');
    }

    public static function assertStatic(bool $cond): string {
        return "CTRL:" . ($cond ? 'true' : 'false');
    }
}

class StaticValidator {
    public static function assert(bool $cond, string $label = 'static'): string {
        return "STATIC:$label:" . ($cond ? 'true' : 'false');
    }
}

$v = new Validator();
$ctrl = $v->assertStatic(?);
var_dump($ctrl(false));

$p1 = $v->assert(?, 'inst');
var_dump($p1(false));

$p2 = StaticValidator::assert(?, 'cls');
var_dump($p2(false));

$p3 = $v->assert(...);
var_dump($p3(false, 'fcc'));

// Positive control: real global \assert(?) must still invoke global assert()
$realAssert = \assert(?, 'real-assert-msg');
var_dump($realAssert(true));
try {
    $realAssert(false);
    echo "UNEXPECTED_PASS\n";
} catch (AssertionError $e) {
    echo "REAL_ASSERT_ERR:" . $e->getMessage() . "\n";
}

?>
--EXPECT--
string(10) "CTRL:false"
string(19) "INSTANCE:inst:false"
string(16) "STATIC:cls:false"
string(18) "INSTANCE:fcc:false"
bool(true)
REAL_ASSERT_ERR:real-assert-msg

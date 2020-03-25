--TEST--
Bug #71767 (ReflectionMethod::getDocComment returns the wrong comment)
--FILE--
<?php

/** Correct docblock */
function foo(
    /** wrong docblock */
    $arg
) {
}

class Foo {
    /** Correct docblock */
    public function bar(
        /** wrong docblock */
        $arg
    ) {

    }
}

/** Correct docblock */
$func = function(
    /** wrong docblock */
    $arg
) {
};

$reflectionFunction = new ReflectionFunction('foo');
$reflectionClass = new ReflectionClass(Foo::class);
$reflectionClosure = new ReflectionFunction($func);

echo $reflectionFunction->getDocComment() . PHP_EOL;
echo $reflectionClass->getMethod('bar')->getDocComment() . PHP_EOL;
echo $reflectionClosure->getDocComment() . PHP_EOL;

echo "Done\n";
?>
--EXPECT--
/** Correct docblock */
/** Correct docblock */
/** Correct docblock */
Done

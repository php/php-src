--TEST--
Bug #71767 (ReflectionMethod::getDocComment returns the wrong)
--FILE--
<?php

/**
 * Correct docblock
 */
function foo(
    /** wrong dockblock */
    $arg
) {
}

class Foo {
    /**
     * Correct docblock
     */
    public function bar(
        /** wrong dockblock */
        $arg
    ) {
        
    }
}

/**
 * Correct docblock
 */
$func = static function(
    /** wrong dockblock */
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
--EXPECTF--
/**
 * Correct docblock
 */
/**
     * Correct docblock
     */
/**
 * Correct docblock
 */
Done

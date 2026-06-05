--TEST--
Class name with correct case does not produce deprecation warnings
--FILE--
<?php
class Foo {
    const BAR = 1;
    static int $x = 2;
    static function sm(): string { return 'ok'; }
}

// All of these use correct case or keywords - no deprecation should appear

$o = new Foo();

echo Foo::sm() . "\n";
echo Foo::BAR . "\n";
echo Foo::$x . "\n";
var_dump($o instanceof Foo);
var_dump(class_exists('Foo'));
var_dump(is_a($o, 'Foo'));

try {
    throw new RuntimeException();
} catch (RuntimeException $e) {
    echo "caught\n";
}

// Anonymous class must not warn
$anon = new class {};
var_dump(class_exists(get_class($anon)));

echo "No deprecations\n";
?>
--EXPECT--
ok
1
2
bool(true)
bool(true)
bool(true)
caught
bool(true)
No deprecations

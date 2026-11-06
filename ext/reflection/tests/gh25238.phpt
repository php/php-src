--TEST--
GH-25238 (ReflectionProperty::isInitialized() does not initialize lazy objects once its cache slot is warm)
--FILE--
<?php

class Line {
    public string $label = '';
}

class NoDefault {
    public string $label;
}

$class = new ReflectionClass(Line::class);
$property = new ReflectionProperty(Line::class, 'label');

foreach (['first', 'second', 'third'] as $label) {
    $line = $class->newLazyGhost(function (Line $line) use ($label) {
        $line->label = $label;
    });
    var_dump($property->isInitialized($line));
    var_dump($class->isUninitializedLazyObject($line));
    var_dump($line->label);
}

foreach (['first', 'second'] as $label) {
    $line = $class->newLazyProxy(function () use ($label) {
        $line = new Line();
        $line->label = $label;
        return $line;
    });
    var_dump($property->isInitialized($line));
    var_dump($class->isUninitializedLazyObject($line));
    var_dump($property->isInitialized($line));
    var_dump($line->label);
}

$line = new Line();
unset($line->label);
var_dump($property->isInitialized($line));

$class = new ReflectionClass(NoDefault::class);
$property = new ReflectionProperty(NoDefault::class, 'label');

for ($i = 0; $i < 2; $i++) {
    $object = $class->newLazyGhost(function (NoDefault $object) {});
    var_dump($property->isInitialized($object));
    var_dump($class->isUninitializedLazyObject($object));
}

?>
--EXPECT--
bool(true)
bool(false)
string(5) "first"
bool(true)
bool(false)
string(6) "second"
bool(true)
bool(false)
string(5) "third"
bool(true)
bool(false)
bool(true)
string(5) "first"
bool(true)
bool(false)
bool(true)
string(6) "second"
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

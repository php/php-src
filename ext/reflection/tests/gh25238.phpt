--TEST--
GH-25238 (ReflectionProperty::isInitialized() does not initialize lazy objects once its cache slot is warm)
--FILE--
<?php

class Line {
    public string $label = '';
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

$line = new Line();
unset($line->label);
var_dump($property->isInitialized($line));

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
bool(false)

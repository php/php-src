--TEST--
Collator attribute and strength methods on unconstructed object
--EXTENSIONS--
intl
--FILE--
<?php

class Collator2 extends Collator {
    public function __construct() {
        // omitting parent::__construct($someLocale);
    }
}

$c = new Collator2();

$methods = [
    'getAttribute' => fn() => $c->getAttribute(Collator::NUMERIC_COLLATION),
    'setAttribute' => fn() => $c->setAttribute(Collator::NUMERIC_COLLATION, Collator::ON),
    'getStrength' => fn() => $c->getStrength(),
    'setStrength' => fn() => $c->setStrength(Collator::SECONDARY),
];

foreach ($methods as $method => $call) {
    try {
        $call();
    } catch (Error $e) {
        echo $method, ': ', $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

$functions = [
    'collator_get_attribute' => fn() => collator_get_attribute($c, Collator::NUMERIC_COLLATION),
    'collator_set_attribute' => fn() => collator_set_attribute($c, Collator::NUMERIC_COLLATION, Collator::ON),
    'collator_get_strength' => fn() => collator_get_strength($c),
    'collator_set_strength' => fn() => collator_set_strength($c, Collator::SECONDARY),
];

foreach ($functions as $function => $call) {
    try {
        $call();
    } catch (Error $e) {
        echo $function, ': ', $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
getAttribute: Error: Object not initialized
setAttribute: Error: Object not initialized
getStrength: Error: Object not initialized
setStrength: Error: Object not initialized
collator_get_attribute: Error: Object not initialized
collator_set_attribute: Error: Object not initialized
collator_get_strength: Error: Object not initialized
collator_set_strength: Error: Object not initialized

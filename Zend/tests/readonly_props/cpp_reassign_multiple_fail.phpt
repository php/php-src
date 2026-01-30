--TEST--
Promoted readonly property reassignment in constructor - multiple reassignments fail
--FILE--
<?php

class Example {
    public function __construct(
        public readonly string $value = 'default',
    ) {
        $this->value = 'first';   // OK - first reassignment
        try {
            $this->value = 'second';  // Error - second reassignment
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

$ex = new Example();
var_dump($ex->value);

?>
--EXPECT--
Cannot modify readonly property Example::$value
string(5) "first"

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
        } catch (Throwable $e) {
            echo get_class($e), ": ", $e->getMessage(), "\n";
        }
    }
}

$ex = new Example();
var_dump($ex->value);

?>
--EXPECT--
Error: Cannot modify readonly property Example::$value
string(5) "first"

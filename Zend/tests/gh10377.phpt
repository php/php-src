--TEST--
GH-10377 (Unable to have an anonymous readonly class)
--FILE--
<?php

$readonly_anon = new readonly class {
    public int $field;
    function __construct() {
        $this->field = 2;
    }
};

$anon = new class {
    public int $field;
    function __construct() {
        $this->field = 2;
    }
};

var_dump($readonly_anon->field);
try {
    $readonly_anon->field = 123;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
var_dump($readonly_anon->field);

var_dump($anon->field);
try {
    $anon->field = 123;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
var_dump($anon->field);

?>
--EXPECT--
int(2)
Cannot modify readonly property class@anonymous::$field
int(2)
int(2)
int(123)

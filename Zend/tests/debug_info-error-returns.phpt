--TEST--
Testing __debugInfo() magic method with bad returns
--FILE--
<?php

class C {
    public $val;
    public function __debugInfo() {
        return $this->val;
    }
    public function __construct($val) {
        $this->val = $val;
    }
}

$VALUES = [
    false,
    true,
    0,
    0.0,
    1.0,
    1,
    "",
    "foo",
    new stdClass(),
    STDERR,
];

foreach ($VALUES as $value) {
    $c = new C($value);

    try {
        var_dump($c);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}
?>
--EXPECT--
TypeError: __debuginfo() must return an array
TypeError: __debuginfo() must return an array
TypeError: __debuginfo() must return an array
TypeError: __debuginfo() must return an array
TypeError: __debuginfo() must return an array
TypeError: __debuginfo() must return an array
TypeError: __debuginfo() must return an array
TypeError: __debuginfo() must return an array
TypeError: __debuginfo() must return an array
TypeError: __debuginfo() must return an array

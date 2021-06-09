--TEST--
Autovivification of false to array
--FILE--
<?php

// no
$undef[] = 42;

// no
$null = null;
$null[] = 42;

// yes
$false = false;
$false[] = 42;

// yes
$ref = false;
$ref2 = &$ref;
$ref2[] = 42;

echo "now function\n";
function ffalse(bool $a, ?bool $b, &$c, ...$d) {
    // yes
    $a[] = 42;
    // yes
    $b[] = 42;
    // yes
    $c[] = 42;
    // yes
    $d[0][] = 42;
}
$ref = false;
ffalse(false, false, $ref, false);

echo "now class\n";
class Cfalse {
    public $def;
    private $untyped = false;
    static private $st = false;
    static private $st2 = false;
    static private $st3 = false;
    public function __construct(public $pu, private $pr = false) {
        // yes
        $this->def = false;
        $this->def[] = 42;
        // yes
        $this->untyped[] = 42;
        // yes
        self::$st[] = 42;
        // yes
        static::$st2[] = 42;
        // yes
        $this::$st3[] = 42;
        // yes
        $this->pu[] = 42;
        // yes
        $this->pr[] = 42;
    }
}
new Cfalse(false, false);

?>
--EXPECTF--
Deprecated: Automatic conversion of false to array is deprecated in %s

Deprecated: Automatic conversion of false to array is deprecated in %s
now function

Deprecated: Automatic conversion of false to array is deprecated in %s

Deprecated: Automatic conversion of false to array is deprecated in %s

Deprecated: Automatic conversion of false to array is deprecated in %s

Deprecated: Automatic conversion of false to array is deprecated in %s
now class

Deprecated: Automatic conversion of false to array is deprecated in %s on line 43

Deprecated: Automatic conversion of false to array is deprecated in %s on line 45

Deprecated: Automatic conversion of false to array is deprecated in %s on line 47

Deprecated: Automatic conversion of false to array is deprecated in %s on line 49

Deprecated: Automatic conversion of false to array is deprecated in %s on line 51

Deprecated: Automatic conversion of false to array is deprecated in %s on line 53

Deprecated: Automatic conversion of false to array is deprecated in %s on line 55

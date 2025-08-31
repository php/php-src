--TEST--
Autovivification of false to array
--FILE--
<?php

// control
$undef[] = 42;

// control
$null = null;
$null[] = 42;

// control
$false = false;
$false = [42];

print "[001]\n";
$false = false;
$false[] = 42;

print "[002]\n";
$ref = false;
$ref2 = &$ref;
$ref2[] = 42;

echo "\nFunction\n";
function ffalse(bool $a, ?bool $b, &$c, ...$d) {
    print "[003]\n";
    $a[] = 42;
    print "[004]\n";
    $b[] = 42;
    print "[005]\n";
    $c[] = 42;
    print "[006]\n";
    $d[0][] = 42;
}
$ref = false;
ffalse(false, false, $ref, false);

echo "\nProperties\n";
class Cfalse {
    public $def;
    private $untyped = false;
    static private $st = false;
    static private $st2 = false;
    static private $st3 = false;
    public function __construct(public $pu, private $pr = false) {
        print "[007]\n";
        $this->def = false;
        $this->def[] = 42;
        print "[008]\n";
        $this->untyped[] = 42;
        print "[009]\n";
        self::$st[] = 42;
        print "[010]\n";
        static::$st2[] = 42;
        print "[011]\n";
        $this::$st3[] = 42;
        print "[012]\n";
        $this->pu[] = 42;
        print "[013]\n";
        $this->pr[] = 42;
    }
}
new Cfalse(false, false);

echo "\nDestructuring\n";

print "[014]\n";
$add = false;
foreach ([42] as $add[]);

print "[015]\n";
$arr = false;
[$arr[]] = [42];

print "[016]\n";
$arr = [ 0 => [ 0 => false ] ];
$arr[0][0][0][] = 42;

print "[017]\n";
$false = false;
$r42 = 42;
$false[] &= $r42;

$false = false;
$false2 = false;
$false3 = false;
function &g(){
    print "[018]\n";
    global $false;
    $false[] = 42;

    $var1 = false;
    $GLOBALS["false2"] =& $var1;

    print "[019]\n";
    $GLOBALS["false3"][] = 42;

    print "[020]\n";
    static $f2 = false;
    return $f2;
}

$false = &g();
$false[] = 42;
print "[021]\n";
$false2[] = 42;

print "[022]\n";
$a = false;
unset($a[0][0]);

print "[023]\n";
$a = false;
unset($a[0]);

?>
--EXPECTF--
[001]

Deprecated: Automatic conversion of false to array is deprecated in %s
[002]

Deprecated: Automatic conversion of false to array is deprecated in %s

Function
[003]

Deprecated: Automatic conversion of false to array is deprecated in %s
[004]

Deprecated: Automatic conversion of false to array is deprecated in %s
[005]

Deprecated: Automatic conversion of false to array is deprecated in %s
[006]

Deprecated: Automatic conversion of false to array is deprecated in %s

Properties
[007]

Deprecated: Automatic conversion of false to array is deprecated in %s
[008]

Deprecated: Automatic conversion of false to array is deprecated in %s
[009]

Deprecated: Automatic conversion of false to array is deprecated in %s
[010]

Deprecated: Automatic conversion of false to array is deprecated in %s
[011]

Deprecated: Automatic conversion of false to array is deprecated in %s
[012]

Deprecated: Automatic conversion of false to array is deprecated in %s
[013]

Deprecated: Automatic conversion of false to array is deprecated in %s

Destructuring
[014]

Deprecated: Automatic conversion of false to array is deprecated in %s
[015]

Deprecated: Automatic conversion of false to array is deprecated in %s
[016]

Deprecated: Automatic conversion of false to array is deprecated in %s
[017]

Deprecated: Automatic conversion of false to array is deprecated in %s
[018]

Deprecated: Automatic conversion of false to array is deprecated in %s
[019]

Deprecated: Automatic conversion of false to array is deprecated in %s
[020]

Deprecated: Automatic conversion of false to array is deprecated in %s
[021]

Deprecated: Automatic conversion of false to array is deprecated in %s
[022]

Deprecated: Automatic conversion of false to array is deprecated in %s
[023]

Deprecated: Automatic conversion of false to array is deprecated in %s

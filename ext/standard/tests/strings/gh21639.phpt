--TEST--
GH-21639 (Volatile string arguments in frameless calls)
--FILE--
<?php
class Clobber {
    public function __construct(private string $var, private string $ret) {}

    public function __toString(): string {
        $GLOBALS[$this->var] = null;
        return $this->ret;
    }
}

class Modify {
    public function __toString(): string {
        global $a;
        $a[0] = '!';
        return '0';
    }
}

$a = str_repeat('foo', 2);
var_dump(strtr($a, 'o', new Modify));
var_dump($a);

$a = str_repeat('foo', 2);
var_dump(strtr($a, 'o', new Clobber('a', 'x')));
var_dump($a);

$a = str_repeat('ab', 2);
var_dump(str_contains($a, new Clobber('a', 'b')));

$a = str_repeat('ab', 2);
var_dump(strpos($a, new Clobber('a', 'b')));

$sep = str_repeat('-', 2);
var_dump(implode($sep, [new Clobber('sep', 'x'), 'y']));

set_error_handler(function (int $errno, string $errstr) {
    $GLOBALS['a'] = null;
    echo $errstr, PHP_EOL;
    return true;
});
$a = str_repeat('ab', 2);
var_dump(substr($a, 1.5));
restore_error_handler();

spl_autoload_register(function (string $class) {
    $GLOBALS['p'] = null;
    eval("class $class { public \$xx; }");
});
$p = str_repeat('x', 2);
var_dump(property_exists('Autoloaded', $p));
?>
--EXPECT--
string(6) "f00f00"
string(6) "!oofoo"
string(6) "fxxfxx"
NULL
bool(true)
int(1)
string(4) "x--y"
Implicit conversion from float 1.5 to int loses precision
string(3) "bab"
bool(true)

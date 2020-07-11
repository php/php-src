--TEST--
foreach() must return properly typed references
--FILE--
<?php
class Foo {
    public int $bar = 0;
    public float $baz = 0.5;
    private float $privateProp = 0.5;

    public function test() {
        foreach ($this as $k => &$val) {
            if ($k == 'privateProp') {
                var_dump($val);
                $val = 20;
                var_dump($val);
                try {
                    $val = [];
                } catch (Error $e) {
                    echo $e->getMessage(), "\n";
                }
            }
        }
    }
}

$foo = new Foo;
foreach ($foo as $k => &$val) {
    var_dump($val);

    $val = 20;
    var_dump($foo->$k);

    try {
        $val = [];
        var_dump($foo->$k);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
$foo->test();
?>
--EXPECT--
int(0)
int(20)
Cannot assign array to reference held by property Foo::$bar of type int
float(0.5)
float(20)
Cannot assign array to reference held by property Foo::$baz of type float
float(0.5)
float(20)
Cannot assign array to reference held by property Foo::$privateProp of type float

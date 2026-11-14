--TEST--
GH-24743 (var_export() does not handle exception correctly when retrieving object)
--FILE--
<?php

class Inner {
    public static bool $fail = true;

    public int $x = 1 {
        get {
            if (self::$fail) {
                throw new Exception("hook failed");
            }
            return $this->x;
        }
    }
}

class Outer {
    public Inner $a;
    public int $b = 2;

    public function __construct() {
        $this->a = new Inner;
    }
}

$o = new Outer;

function show_export(string $label, callable $export): void
{
    ob_start();
    $caught = null;
    try {
        $export();
    } catch (Throwable $e) {
        $caught = $e;
    }
    $output = ob_get_clean();

    echo "--- ", $label, " ---\n";
    if ($caught === null) {
        echo "No exception\n";
    } else {
        echo "Caught: ", $caught->getMessage(), "\n";
    }

    if ($output !== '') {
        echo "BUG: var_export() emitted partial output before the exception:\n";
        echo $output, "\n";
        echo "--- end of partial output ---\n";
    }
}

show_export("print mode, nested object", fn() => var_export($o));
show_export("return mode, nested object", fn() => var_export($o, true));
show_export("print mode, object inside array", fn() => var_export([1, $o]));

echo "--- recovery: recursion guards were released ---\n";
Inner::$fail = false;
var_export($o);
echo "\n";

?>
--EXPECT--
--- print mode, nested object ---
Caught: hook failed
--- return mode, nested object ---
Caught: hook failed
--- print mode, object inside array ---
Caught: hook failed
--- recovery: recursion guards were released ---
\Outer::__set_state(array(
   'a' => 
  \Inner::__set_state(array(
     'x' => 1,
  )),
   'b' => 2,
))

--TEST--
GC 050: Test destructor is never run twice
--FILE--
<?php

class G
{
    public static $v;
}

class WithDestructor
{
    public function __destruct()
    {
        echo "d\n";

        G::$v = $this;
    }
}

$o = new WithDestructor();
$weakO = \WeakReference::create($o);
echo "---\n";
unset($o);
echo "---\n";
var_dump($weakO->get() !== null); // verify if kept allocated
G::$v = null;
echo "---\n";
var_dump($weakO->get() !== null); // verify if released
?>
--EXPECT--
---
d
---
bool(true)
---
bool(false)

--TEST--
GC 050: Destructor are never called twice
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
var_dump($weakO->get() !== null); // weakref severed at the drop; the deferred dtor re-referencing $this does not restore it
G::$v = null;
echo "---\n";
var_dump($weakO->get() !== null); // still severed
?>
--EXPECT--
---
---
d
bool(false)
---
bool(false)

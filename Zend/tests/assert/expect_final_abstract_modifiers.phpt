--TEST--
AST printing of final and abstract modifiers on class constants and properties
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert(false && new class {
        final public const A = 1;
        final protected const int B = 2;
        final public $a;
        final public static $b;
        final public readonly int $c;
        abstract public $d { get; }
        abstract protected private(set) int $e { set; }
    });
} catch (AssertionError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
assert(false && new class {
    public final const A = 1;
    protected final const int B = 2;
    public final $a;
    public static final $b;
    public final readonly int $c;
    public abstract $d {
        get;
    }
    protected private(set) abstract int $e {
        set;
    }
})

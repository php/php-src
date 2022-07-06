--TEST--
Test ReflectionProperty::getDocComment() usage.
--INI--
opcache.save_comments=1
--FILE--
<?php

class A {
    /**
     * My Doc Comment for $a
     *
     */
    public $a = 2, $b, $c = 1;
    /**
     * My Doc Comment for $d
     */
    var $d;
    /**Not a doc comment */
    private $e;
    /**
     * Doc comment for $f
     */
    static protected $f;
}

class B extends A {
    public $a = 2;
    /** A doc comment for $b */
    var $b, $c = 1;
    /** A doc comment for $e */
    var $e;
}

foreach(array('A', 'B') as $class) {
    $rc = new ReflectionClass($class);
    $rps = $rc->getProperties();
    foreach($rps as $rp) {
        echo "\n\n---> Doc comment for $class::$" . $rp->getName() . ":\n";
        var_dump($rp->getDocComment());
    }
}

?>
--EXPECTF--
---> Doc comment for A::$a:
string(%d) "/**
     * My Doc Comment for $a
     *
     */"


---> Doc comment for A::$b:
bool(false)


---> Doc comment for A::$c:
bool(false)


---> Doc comment for A::$d:
string(%d) "/**
     * My Doc Comment for $d
     */"


---> Doc comment for A::$e:
bool(false)


---> Doc comment for A::$f:
string(%d) "/**
     * Doc comment for $f
     */"


---> Doc comment for B::$a:
bool(false)


---> Doc comment for B::$b:
string(%d) "/** A doc comment for $b */"


---> Doc comment for B::$c:
bool(false)


---> Doc comment for B::$e:
string(%d) "/** A doc comment for $e */"


---> Doc comment for B::$d:
string(%d) "/**
     * My Doc Comment for $d
     */"


---> Doc comment for B::$f:
string(%d) "/**
     * Doc comment for $f
     */"

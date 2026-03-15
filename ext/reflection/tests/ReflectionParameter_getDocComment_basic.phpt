--TEST--
Test ReflectionParameter::getDocComment() usage.
--INI--
opcache.save_comments=1
--FILE--
<?php

class A
{
function func(
    /**
     * My Doc Comment for $a
     *
     */
    $a, $b, $c,
    /**
     * My Doc Comment for $d
     */
    $d,
    // Not a doc comment
    /**Not a doc comment */
    $e,
    /**
     * Doc comment for $f
     */
    $f,
	$g /** Doc comment for $g behind parameter */,
	/** Doc comment for $h */
	$h /** Doc comment for $h behind parameter */,
) {}
}

function func(
    /**
     * My Doc Comment for $a
     *
     */
    $a, $b, $c,
    /**
     * My Doc Comment for $d
     */
    $d,
    // Not a doc comment
    /**Not a doc comment */
    $e,
    /**
     * Doc comment for $f
     */
    $f,
	$g /** Doc comment for $g behind parameter */,
	/** Doc comment for $h */
	$h /** Doc comment for $h behind parameter */,
) {}

$func = function(
    /**
     * My Doc Comment for $a
     *
     */
    $a, $b, $c,
    /**
     * My Doc Comment for $d
     */
    $d,
    // Not a doc comment
    /**Not a doc comment */
    $e,
    /**
     * Doc comment for $f
     */
    $f,
	$g /** Doc comment for $g behind parameter */,
	/** Doc comment for $h */
	$h /** Doc comment for $h behind parameter */,
) {};

foreach([
		'A::func' => (new ReflectionClass('A'))->getMethod('func'),
		'func'    => new ReflectionFunction('func'),
		'closure' => new ReflectionFunction($func),
	] as $function => $rc) {
    $rps = $rc->getParameters();
    foreach($rps as $rp) {
        echo "\n---> Doc comment for $function parameter $" . $rp->getName() . ":\n";
        var_dump($rp->getDocComment());
    }
}

?>
--EXPECTF--
---> Doc comment for A::func parameter $a:
string(%d) "/**
     * My Doc Comment for $a
     *
     */"

---> Doc comment for A::func parameter $b:
bool(false)

---> Doc comment for A::func parameter $c:
bool(false)

---> Doc comment for A::func parameter $d:
string(%d) "/**
     * My Doc Comment for $d
     */"

---> Doc comment for A::func parameter $e:
bool(false)

---> Doc comment for A::func parameter $f:
string(%d) "/**
     * Doc comment for $f
     */"

---> Doc comment for A::func parameter $g:
string(%d) "/** Doc comment for $g behind parameter */"

---> Doc comment for A::func parameter $h:
string(%d) "/** Doc comment for $h behind parameter */"

---> Doc comment for func parameter $a:
string(%d) "/**
     * My Doc Comment for $a
     *
     */"

---> Doc comment for func parameter $b:
bool(false)

---> Doc comment for func parameter $c:
bool(false)

---> Doc comment for func parameter $d:
string(%d) "/**
     * My Doc Comment for $d
     */"

---> Doc comment for func parameter $e:
bool(false)

---> Doc comment for func parameter $f:
string(%d) "/**
     * Doc comment for $f
     */"

---> Doc comment for func parameter $g:
string(%d) "/** Doc comment for $g behind parameter */"

---> Doc comment for func parameter $h:
string(%d) "/** Doc comment for $h behind parameter */"

---> Doc comment for closure parameter $a:
string(%d) "/**
     * My Doc Comment for $a
     *
     */"

---> Doc comment for closure parameter $b:
bool(false)

---> Doc comment for closure parameter $c:
bool(false)

---> Doc comment for closure parameter $d:
string(%d) "/**
     * My Doc Comment for $d
     */"

---> Doc comment for closure parameter $e:
bool(false)

---> Doc comment for closure parameter $f:
string(%d) "/**
     * Doc comment for $f
     */"

---> Doc comment for closure parameter $g:
string(%d) "/** Doc comment for $g behind parameter */"

---> Doc comment for closure parameter $h:
string(%d) "/** Doc comment for $h behind parameter */"


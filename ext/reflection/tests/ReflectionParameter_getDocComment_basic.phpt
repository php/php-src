--TEST--
Test ReflectionParameter::getDocComment() usage.
--INI--
opcache.save_comments=1
--FILE--
<?php

class A
{
function method(
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
	$g /** Doc comment for $g after parameter */,
	/** Doc comment for $h */
	$h /** Doc comment for $h after parameter */,
) {}

public string $property {
     set(
          /** Doc Comment for property hook parameter $value */
          string $value
     ) { $this->property = $value; }
}
}

function global_function(
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
	$g /** Doc comment for $g after parameter */,
	/** Doc comment for $h */
	$h /** Doc comment for $h after parameter */,
) {}

$closure = function(
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
	$g /** Doc comment for $g after parameter */,
	/** Doc comment for $h */
	$h /** Doc comment for $h after parameter */,
) {};

$arrow_function = fn(
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
	$g /** Doc comment for $g after parameter */,
	/** Doc comment for $h */
	$h /** Doc comment for $h after parameter */,
) => true;

foreach([
		'A::method'       => (new ReflectionClass('A'))->getMethod('method'),
		'global_function' => new ReflectionFunction('global_function'),
		'closure'         => new ReflectionFunction($closure),
		'arrow_function'  => new ReflectionFunction($arrow_function),
		'property hook'   => (new ReflectionClass('A'))->getProperty('property')->getHook(PropertyHookType::Set),
	] as $function => $rc) {
    $rps = $rc->getParameters();
    foreach($rps as $rp) {
        echo "\n---> Doc comment for $function parameter $" . $rp->getName() . ":\n";
        var_dump($rp->getDocComment());
    }
}

?>
--EXPECTF--
---> Doc comment for A::method parameter $a:
string(%d) "/**
     * My Doc Comment for $a
     *
     */"

---> Doc comment for A::method parameter $b:
bool(false)

---> Doc comment for A::method parameter $c:
bool(false)

---> Doc comment for A::method parameter $d:
string(%d) "/**
     * My Doc Comment for $d
     */"

---> Doc comment for A::method parameter $e:
bool(false)

---> Doc comment for A::method parameter $f:
string(%d) "/**
     * Doc comment for $f
     */"

---> Doc comment for A::method parameter $g:
string(%d) "/** Doc comment for $g after parameter */"

---> Doc comment for A::method parameter $h:
string(%d) "/** Doc comment for $h after parameter */"

---> Doc comment for global_function parameter $a:
string(%d) "/**
     * My Doc Comment for $a
     *
     */"

---> Doc comment for global_function parameter $b:
bool(false)

---> Doc comment for global_function parameter $c:
bool(false)

---> Doc comment for global_function parameter $d:
string(%d) "/**
     * My Doc Comment for $d
     */"

---> Doc comment for global_function parameter $e:
bool(false)

---> Doc comment for global_function parameter $f:
string(%d) "/**
     * Doc comment for $f
     */"

---> Doc comment for global_function parameter $g:
string(%d) "/** Doc comment for $g after parameter */"

---> Doc comment for global_function parameter $h:
string(%d) "/** Doc comment for $h after parameter */"

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
string(%d) "/** Doc comment for $g after parameter */"

---> Doc comment for closure parameter $h:
string(%d) "/** Doc comment for $h after parameter */"

---> Doc comment for arrow_function parameter $a:
string(%d) "/**
     * My Doc Comment for $a
     *
     */"

---> Doc comment for arrow_function parameter $b:
bool(false)

---> Doc comment for arrow_function parameter $c:
bool(false)

---> Doc comment for arrow_function parameter $d:
string(%d) "/**
     * My Doc Comment for $d
     */"

---> Doc comment for arrow_function parameter $e:
bool(false)

---> Doc comment for arrow_function parameter $f:
string(%d) "/**
     * Doc comment for $f
     */"

---> Doc comment for arrow_function parameter $g:
string(%d) "/** Doc comment for $g after parameter */"

---> Doc comment for arrow_function parameter $h:
string(%d) "/** Doc comment for $h after parameter */"

---> Doc comment for property hook parameter $value:
string(%d) "/** Doc Comment for property hook parameter $value */"


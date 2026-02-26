--TEST--
Test ReflectionParameter::getDocComment() usage.
--INI--
opcache.save_comments=1
--FILE--
<?php

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
    /**Not a doc comment */
    $e,
    /**
     * Doc comment for $f
     */
    $f,
	$g /** Doc comment for $g behind parameter */,
) {}

foreach(['func'] as $function) {
    $rc = new ReflectionFunction($function);
    $rps = $rc->getParameters();
    foreach($rps as $rp) {
        echo "\n\n---> Doc comment for $function $" . $rp->getName() . ":\n";
        var_dump($rp->getDocComment());
    }
}

?>
--EXPECTF--
---> Doc comment for func $a:
string(%d) "/**
     * My Doc Comment for $a
     *
     */"


---> Doc comment for func $b:
bool(false)


---> Doc comment for func $c:
bool(false)


---> Doc comment for func $d:
string(%d) "/**
     * My Doc Comment for $d
     */"


---> Doc comment for func $e:
bool(false)


---> Doc comment for func $f:
string(%d) "/**
     * Doc comment for $f
     */"


---> Doc comment for func $g:
string(%d) "/** Doc comment for $g behind parameter */"



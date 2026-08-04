--TEST--
Test ReflectionParameter::getDocComment() usage when methods are indented.
--INI--
opcache.save_comments=1
--FILE--
<?php

class A {
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

foreach([
        'A::method'       => (new ReflectionClass('A'))->getMethod('method'),
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

---> Doc comment for property hook parameter $value:
string(%d) "/** Doc Comment for property hook parameter $value */"

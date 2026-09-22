--TEST--
GH-20906 (Assertion failure when messing up output buffers) - php_strip_whitespace
--CREDITS--
vi3tL0u1s
--FILE--
<?php
class A {
    function __destruct() {
        php_strip_whitespace(__FILE__);
        echo "x";
        $c = new A;
        ob_start(function () use ($c) { return '/'; }, 1);
        ob_start(function () use (&$c) { $c = new A; return '/'; }, 1);
    }
}

try {
  new A;
} catch (Throwable $e) {
  echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
%a
Fatal error: php_strip_whitespace(): Cannot use output buffering in output buffering display handlers in %s on line %d

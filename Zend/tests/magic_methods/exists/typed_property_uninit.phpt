--TEST--
__exists: skipped for never-initialized typed properties (parity with __isset); unset() is the opt-in
--FILE--
<?php

/* For never-initialized typed properties, __exists is skipped, exactly
 * like __isset. To opt magic methods in for a declared property, call
 * unset() on it (typically from the constructor). This mirrors the
 * existing lazy-proxy pattern used with __isset. */

class T {
    public int $x;

    public function __exists(string $n): bool {
        echo "  __exists($n)\n";
        $this->$n = 42;
        return true;
    }
    public function __get(string $n): mixed {
        throw new Exception("__get must not be called when __exists materialised the property");
    }
}

echo "1) Never-initialized typed property: __exists is NOT called (parity with __isset):\n";
$t = new T;
var_dump($t->x ?? 'fallback');
var_dump(isset($t->x));

echo "\n2) After unset(), magic methods kick in and __exists materializes the property:\n";
$t = new T;
unset($t->x);
var_dump($t->x ?? 'fallback');
var_dump(isset($t->x));

echo "\n3) Subsequent access on the now-materialized property does not call any magic:\n";
var_dump($t->x);

?>
--EXPECT--
1) Never-initialized typed property: __exists is NOT called (parity with __isset):
string(8) "fallback"
bool(false)

2) After unset(), magic methods kick in and __exists materializes the property:
  __exists(x)
int(42)
bool(true)

3) Subsequent access on the now-materialized property does not call any magic:
int(42)

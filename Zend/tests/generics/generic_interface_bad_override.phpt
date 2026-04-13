--TEST--
Generic interface: incompatible implementation detected
--FILE--
<?php
interface Transformer<T> {
    public function transform(T $input): T;
}

// string is not compatible with int for Transformer<int>
class BadTransformer implements Transformer<int> {
    public function transform(string $input): string { return $input; }
}
?>
--EXPECTF--
Fatal error: Declaration of BadTransformer::transform(string $input): string must be compatible with Transformer::transform(T $input): T in %s on line %d

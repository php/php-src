--TEST--
Test typed properties initial values
--FILE--
<?php
class Foo {
	public int $int = 1;
	public float $flt = 2.2;
    public float $flt2 = 2;
	public array $arr = [];
	public bool $bool = false;
    public iterable $iter = [];
}
echo "ok\n";
?>
--EXPECT--
ok

--TEST--
Diamond + nullable: contravariant union propagates null from any operand
--FILE--
<?php
interface Setter<in X> { public function set(X $v): void; }

interface Flex extends Setter<?int>, Setter<string> {}
$r = new ReflectionClass(Flex::class);
echo "merged: ", (string)$r->getMethod('set')->getParameters()[0]->getType(), "\n";

class Good implements Flex {
    public function set(int|string|null $v): void {}
}
echo "good ok\n";
?>
--EXPECT--
merged: string|int|null
good ok

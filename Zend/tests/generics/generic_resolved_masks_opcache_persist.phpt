--TEST--
Generic class: resolved_masks persistence through opcache SHM and file cache
--DESCRIPTION--
Tests that resolved_masks are correctly persisted through opcache shared
memory and that generic type checking still works after the masks are
restored from SHM.
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
declare(strict_types=1);

class Container<T> {
    public T $item;
    public function __construct(T $item) { $this->item = $item; }
    public function get(): T { return $this->item; }
    public function set(T $item): void { $this->item = $item; }
}

// Scalar types (mask fast path through persisted masks)
$intC = new Container<int>(1);
$strC = new Container<string>("a");
$fltC = new Container<float>(1.5);
$boolC = new Container<bool>(true);

echo $intC->get() . "\n";
echo $strC->get() . "\n";
echo $fltC->get() . "\n";
var_dump($boolC->get());

// Verify type enforcement still works with persisted masks
try { $intC->set("bad"); } catch (TypeError $e) { echo "int reject: OK\n"; }
try { $strC->set(123); } catch (TypeError $e) { echo "str reject: OK\n"; }
try { $fltC->set("bad"); } catch (TypeError $e) { echo "float reject: OK\n"; }

// Property assignment with persisted masks
$intC->item = 42;
echo $intC->item . "\n";
try { $intC->item = "bad"; } catch (TypeError $e) { echo "prop reject: OK\n"; }

// Complex type (mask=0, slow path through persisted masks)
$nested = new Container<Container<int>>(new Container<int>(99));
echo $nested->get()->get() . "\n";

// Default type params with persisted masks
class Pair<K, V = string> {
    public function __construct(public K $key, public V $val) {}
}
$p = new Pair<int>(42, "hello");
echo $p->key . " " . $p->val . "\n";

// Inheritance with persisted bound args
class IntContainer extends Container<int> {}
$ic = new IntContainer(77);
echo $ic->get() . "\n";
try { $ic->set("bad"); } catch (TypeError $e) { echo "inherit reject: OK\n"; }

echo "Done.\n";
?>
--EXPECT--
1
a
1.5
bool(true)
int reject: OK
str reject: OK
float reject: OK
42
prop reject: OK
99
42 hello
77
inherit reject: OK
Done.

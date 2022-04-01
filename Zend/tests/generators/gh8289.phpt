--TEST--
GH-8289 (Exceptions thrown within a yielded from iterator are not rethrown into the generator)
--FILE--
<?php

function yieldFromIteratorGeneratorThrows() {
	try {
		yield from new class(new ArrayIterator([1, -2])) extends IteratorIterator {
			public function key() {
				if ($k = parent::key()) {
					throw new Exception;
				}
				return $k;
			}
		};
	} catch (Exception $e) {
		echo "$e\n";
		yield 2;
	}
}

foreach (yieldFromIteratorGeneratorThrows() as $k => $v) {
    var_dump($v);
}

?>
--EXPECTF--
int(1)
Exception in %s:%d
Stack trace:
#0 %s(%d): IteratorIterator@anonymous->key()
#1 %s(%d): yieldFromIteratorGeneratorThrows()
#2 {main}
int(2)
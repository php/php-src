--TEST--
Bug #70644: hash key complexity DoS on SplObjectStorage
--FILE--
<?php

class NewObjectStorage extends SplObjectStorage {
	function getHash($o) {
		static $g;
		if (!$g) {
			$g = (function() {
				$len = 10;
				$perm = ["E\x7a", "F\x59", "G\x38", "H\x17", "I\xf6", "J\xd5", "K\xb4", "L\x93"];
				$j = 0;
				while (1) {
					$j++;
					$key = "";
					for ($k = 0; $k < $len; $k++) {
						$key .= $perm[($j >> (3 * $k)) % 8];
					}
					yield $key;
				}
			})();
		} else {
			$g->next();
		}
		return $g->current();
	}
}

$o = new SplObjectStorage;
for ($i = 0; $i < 100000; $i++) $o[new StdClass] = $i;

try {
	$n = unserialize(str_replace("SplObjectStorage", "NewObjectStorage", serialize($o)));

	var_dump($n->count());
} catch (Throwable $e) {
	print $e->getMessage();
}

?>
--EXPECT--
Error at offset 26957 of 2888909 bytes

--TEST--
Hash: xxHash secret
--FILE--
<?php

class StringableThrowingClass {
    public function __toString(): string {
        throw new Exception('exception in __toString');
        return '';
    }
}

foreach (["xxh3", "xxh128"] as $a) {

	//$secret = random_bytes(256);
	$secret = str_repeat('a', 256);

	try {
		$ctx = hash_init($a, options: ["seed" => 24, "secret" => $secret]);
	} catch (Throwable $e) {
		var_dump($e->getMessage());
	}

	try {
		$ctx = hash_init($a, options: ["secret" => new StringableThrowingClass()]);
	} catch (Throwable $e) {
		var_dump($e->getMessage());
	}

	try {
		$ctx = hash_init($a, options: ["secret" => str_repeat('a', 17)]);
	} catch (Throwable $e) {
		var_dump($e->getMessage());
	}

	$ctx = hash_init($a, options: ["secret" => $secret]);
	hash_update($ctx, "Lorem");
	hash_update($ctx, " ipsum dolor");
	hash_update($ctx, " sit amet,");
	hash_update($ctx, " consectetur adipiscing elit.");
	$h0 = hash_final($ctx);

	$h1 = hash($a, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.", options: ["secret" => $secret]);
	echo $h0 , " == ", $h1, " == ", (($h0 == $h1) ? "true" : "false"), "\n";

}

?>
--EXPECT--
string(67) "xxh3: Only one of seed or secret is to be passed for initialization"
string(23) "exception in __toString"
string(57) "xxh3: Secret length must be >= 136 bytes, 17 bytes passed"
8028aa834c03557a == 8028aa834c03557a == true
string(69) "xxh128: Only one of seed or secret is to be passed for initialization"
string(23) "exception in __toString"
string(59) "xxh128: Secret length must be >= 136 bytes, 17 bytes passed"
54279097795e7218093a05d4d781cbb9 == 54279097795e7218093a05d4d781cbb9 == true

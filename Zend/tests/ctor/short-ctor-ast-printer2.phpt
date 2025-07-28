--TEST--
Pass short ctor parameters
--FILE--
<?php

try {
    assert(false && function () {

	class DTO (
		public int $number,
	);

    });
} catch (Throwable $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
assert(false && function () {
    class DTO {
        public function __construct(public int $number) {
        }

    }

})
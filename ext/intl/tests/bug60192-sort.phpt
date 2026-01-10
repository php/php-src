--TEST--
Bug #60192 (SegFault when Collator not constructed properly)
--EXTENSIONS--
intl
--FILE--
<?php

class Collator2 extends Collator{
    public function __construct() {
        // omitting parent::__construct($someLocale);
    }
}

$c = new Collator2();
$a = array('a', 'b');
try {
	$c->sort($a);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Error: Object not initialized

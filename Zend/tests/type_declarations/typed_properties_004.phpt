--TEST--
Test typed properties error condition (type mismatch)
--FILE--
<?php
new class("PHP 7 is better than you, and it knows it ...") {
    public int $int;

    public function __construct(string $string) {
        $this->int = $string;
    }
};
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot assign string to property class@anonymous::$int of type int in %s:%d
Stack trace:
#0 %s(%d): class@anonymous->__construct('PHP 7 is better...')
#1 {main}
  thrown in %s on line %d

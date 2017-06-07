--TEST--
Bug #73989 (PHP 7.1 Segfaults within Symfony test suite)
--FILE--
<?php
class Cycle
{
    private $thing;

    public function __construct()
    {
		$obj = $this;
        $this->thing = function() use($obj) {};
    }

    public function __destruct()
    {
		($this->thing)();
    }

}

for ($i = 0; $i < 10000; ++$i) {
    $obj = new Cycle();
}
echo "OK\n";
?>
--EXPECT--
OK

--TEST--
Random: Randomizer: User Engine results are correctly expanded for getInt()
--FILE--
<?php

$randomizer = new \Random\Randomizer (
    new class () implements \Random\Engine
    {
        public function generate(): string
        {
            return "\x01\x01\x01\x01";
        }
    }
);

var_dump(bin2hex(pack('V', $randomizer->getInt(0, 0xFFFFFF))));

?>
--EXPECT--
string(8) "01010100"

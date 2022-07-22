--TEST--
Random: Randomizer: User Engine results are correctly expanded for getInt()
--FILE--
<?php

$randomizer = new \Random\Randomizer (
    new class () implements \Random\Engine
    {
        public $count = 0;

        public function generate(): string
        {
            return "\x01\x02\x03\x04\x05\x06\x07\x08"[$this->count++];
        }
    }
);

var_dump(bin2hex(pack('V', $randomizer->getInt(0, 0xFFFFFF))));

?>
--EXPECT--
string(8) "01020300"

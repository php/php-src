--TEST--
GH-24532 (unserialize() teardown should reset freed var entries before running destructors)
--FILE--
<?php

class Reenter
{
    public function __destruct()
    {
        @unserialize('a:1:{i:0;');

        $payload = 'a:1020:{';
        for ($i = 0; $i < 1020; $i++) {
            $payload .= "i:$i;i:$i;";
        }
        var_dump(count(unserialize($payload . '}')));
    }
}

$count = 1015;
$payload = 'a:' . ($count + 3) . ':{';

for ($i = 0; $i < $count; $i++) {
    $payload .= "i:$i;i:$i;";
}

$payload .= 'i:88888;i:7;i:99999;O:7:"Reenter":0:{}i:99999;i:42;}';

var_dump(count(@unserialize($payload)));
?>
--EXPECT--
int(1020)
int(1017)

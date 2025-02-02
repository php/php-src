--TEST--
Operators in match
--FILE--
<?php
function test1(array $values) {
    foreach ($values as $key => $value) {
        return match (true) {
            $key == "return key" => $key,
            $key == "return value" => $value,
            $key == "break" => break,
            $key == "continue" => continue,
            default => continue,
        };
    }
    return "default return";
}
echo test1(['a', 'b', 'return key' => 'value']) . PHP_EOL;
echo test1(['return value' => 'value', 'a', 'b']) . PHP_EOL;
echo test1(['continue' => 'value', 'return value' => 'value']) . PHP_EOL;
echo test1(['break' => 'value', 'return value' => 'value']) . PHP_EOL;
echo test1([]) . PHP_EOL;

?>
--EXPECT--
return key
value
value
default return
default return

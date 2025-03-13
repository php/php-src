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

echo '---' . PHP_EOL;

function test2(array $values) {
    $i = 0;
    $recursionGuard = function()use(&$i) {return 10 > $i++;};
    $next = reset($values);
    while ($next != null) {
        $recursionGuard() ?: return "recursion";

        $next = match (true) {
            $next == "return key" => return "key",
            $next == "return value" => return "value",
            $next == "break" => break,
            $next == "continue" => continue,
            default => next($values),
        };
    }
    return $next ?? "default return";
}
var_dump(test2([]));
var_dump(test2(['return key']));
var_dump(test2(['return value']));
var_dump(test2(['break']));
var_dump(test2(['a', 'b', 'break']));
var_dump(test2(['a', 'b', 'continue']));

?>
--EXPECT--
return key
value
value
default return
default return
---
bool(false)
string(3) "key"
string(5) "value"
string(5) "break"
string(5) "break"
string(9) "recursion"

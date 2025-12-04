--TEST--
Error message for non-numaric strings in arithmetic contexts
--FILE--
<?php
declare(strict_types=0);

function foo(int $a) { echo $a; }

function msg(TypeError $e){
    $split = explode(', called in', $e->getMessage(), 2);
    return $split[0];
}

var_dump(1 + "1");
try { var_dump(1 + "hello"); } catch (TypeError $e) { echo $e->getMessage(), "\n"; }
try { var_dump(1 + ""); } catch (TypeError $e) { echo $e->getMessage(), "\n"; }
try { var_dump("hello" + 1); } catch (TypeError $e) { echo $e->getMessage(), "\n"; }
try { var_dump("" +  1); } catch (TypeError $e) { echo $e->getMessage(), "\n"; }

foo("1"); 
echo "\n";
try { foo("hello"); } catch (TypeError $e) { echo msg($e), "\n"; }
try { foo(""); } catch (TypeError $e) { echo msg($e), "\n"; }
?>
===DONE===
--EXPECT--
int(2)
Unsupported operand types: int + non-numeric string
Unsupported operand types: int + empty string
Unsupported operand types: non-numeric string + int
Unsupported operand types: empty string + int
1
foo(): Argument #1 ($a) must be of type int, non-numeric string given
foo(): Argument #1 ($a) must be of type int, empty string given
===DONE===

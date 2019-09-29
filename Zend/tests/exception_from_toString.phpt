--TEST--
Test exceptions thrown from __toString() in various contexts
--FILE--
<?php

class BadStr {
    public function __toString() {
        throw new Exception("Exception");
    }
}

$str = "a";
$num = 42;
$badStr = new BadStr;

try { $x = $str . $badStr; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
try { $x = $badStr . $str; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
try { $x = $str .= $badStr; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
var_dump($str); 
try { $x = $num . $badStr; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
try { $x = $badStr . $num; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
try { $x = $num .= $badStr; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
var_dump($num);

try { $x = $badStr .= $str; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
var_dump($badStr);
try { $x = $badStr .= $badStr; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
var_dump($badStr);

try { $x = "x$badStr"; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
try { $x = "{$badStr}x"; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
try { $x = "$str$badStr"; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
try { $x = "$badStr$str"; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }

try { $x = "x$badStr$str"; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
try { $x = "x$str$badStr"; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
try { $x = "{$str}x$badStr"; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
try { $x = "{$badStr}x$str"; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }

try { $x = (string) $badStr; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }

try { $x = include $badStr; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }

try { echo $badStr; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }

${""} = 42;
try { unset(${$badStr}); }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
var_dump(${""});

unset(${""});
try { $x = ${$badStr}; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }

try { $x = isset(${$badStr}); }
catch (Exception $e) { echo $e->getMessage(), "\n"; }

$obj = new stdClass;
try { $x = $obj->{$badStr} = $str; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
var_dump($obj);

try { $str[0] = $badStr; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
var_dump($str);

$obj = new DateInterval('P1D');
try { $x = $obj->{$badStr} = $str; }
catch (Exception $e) { echo $e->getMessage(), "\n"; }
var_dump(!isset($obj->{""}));

try { strlen($badStr); } catch (Exception $e) { echo "Exception\n"; }
try { substr($badStr, 0); } catch (Exception $e) { echo "Exception\n"; }
try { new ArrayObject([], 0, $badStr); } catch (Exception $e) { echo "Exception\n"; }

?>
--EXPECT--
Exception
Exception
Exception
string(1) "a"
Exception
Exception
Exception
int(42)
Exception
object(BadStr)#1 (0) {
}
Exception
object(BadStr)#1 (0) {
}
Exception
Exception
Exception
Exception
Exception
Exception
Exception
Exception
Exception
Exception
Exception
Exception
int(42)
Exception
Exception
Exception
object(stdClass)#2 (0) {
}
Exception
string(1) "a"
Exception
bool(true)
Exception
Exception
Exception

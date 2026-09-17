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
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { $x = $badStr . $str; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { $x = $str .= $badStr; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump($str);
try { $x = $num . $badStr; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { $x = $badStr . $num; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { $x = $num .= $badStr; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump($num);

try { $x = $badStr .= $str; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump($badStr);
try { $x = $badStr .= $badStr; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump($badStr);

try { $x = "x$badStr"; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { $x = "{$badStr}x"; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { $x = "$str$badStr"; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { $x = "$badStr$str"; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

try { $x = "x$badStr$str"; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { $x = "x$str$badStr"; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { $x = "{$str}x$badStr"; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { $x = "{$badStr}x$str"; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

try { $x = (string) $badStr; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

try { $x = include $badStr; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

try { echo $badStr; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

${""} = 42;
try { unset(${$badStr}); }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump(${""});

unset(${""});
try { $x = ${$badStr}; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

try { $x = isset(${$badStr}); }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

$obj = new stdClass;
try { $x = $obj->{$badStr} = $str; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump($obj);

try { $str[0] = $badStr; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump($str);

$obj = new DateInterval('P1D');
try { $x = $obj->{$badStr} = $str; }
catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump(!isset($obj->{""}));

try { strlen($badStr); } catch (Exception $e) { echo "Exception\n"; }
try { substr($badStr, 0); } catch (Exception $e) { echo "Exception\n"; }
try { new ArrayObject([], 0, $badStr); } catch (Exception $e) { echo "Exception\n"; }

?>
--EXPECT--
Exception: Exception
Exception: Exception
Exception: Exception
string(1) "a"
Exception: Exception
Exception: Exception
Exception: Exception
int(42)
Exception: Exception
object(BadStr)#1 (0) {
}
Exception: Exception
object(BadStr)#1 (0) {
}
Exception: Exception
Exception: Exception
Exception: Exception
Exception: Exception
Exception: Exception
Exception: Exception
Exception: Exception
Exception: Exception
Exception: Exception
Exception: Exception
Exception: Exception
Exception: Exception
int(42)
Exception: Exception
Exception: Exception
Exception: Exception
object(stdClass)#2 (0) {
}
Exception: Exception
string(1) "a"
Exception: Exception
bool(true)
Exception
Exception
Exception

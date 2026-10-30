--TEST--
ErrorException properties are overridden by property hooks
--FILE--
<?php
class MyException extends ErrorException
{
    private bool $modified = false;

    protected $code {
        set($value) {
            if ($this->modified) {
                throw new Exception();
            } else {
                $this->modified = true;

                $this->code = $value;
            }
        }
    }
}

$e = new MyException("foo", 1, E_NOTICE, "file1", 1, new Exception());

try {
    $e->__construct("bar", 2, E_WARNING, "file2", 2, null);
} catch (Exception) {
}

var_dump($e->getMessage());
var_dump($e->getCode());
var_dump($e->getSeverity());
var_dump($e->getFile());
var_dump($e->getLine());
var_dump($e->getPrevious()::class);

?>
--EXPECTF--
string(3) "bar"
int(1)
int(8)
string(5) "file1"
int(1)
string(9) "Exception"

--TEST--
Bug #41075 (memleak when creating default object caused exception)
--FILE--
<?php

function err($errno, $errstr, $errfile, $errline)
{
	    throw new Exception($errstr);
}

set_error_handler("err");

class test {
    function foo() {
        $var = $this->blah->prop = "string";
        var_dump($this->blah);
    }
}

$t = new test;
try {
    $t->foo();
} catch (Exception $e) {
    var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECTF--	
string(40) "Creating default object from empty value"
Done

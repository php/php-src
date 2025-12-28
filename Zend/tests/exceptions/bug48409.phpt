--TEST--
Bug #48409 (crash when exception is thrown while passing function arguments)
--FILE--
<?php

class ABCException extends Exception {}

class BBB
{
    public function xyz($d, $x)
    {
        if ($x == 34) {
            throw new ABCException;
        }
        return array('foo' => 'xyz');
    }
}

class CCC
{
    public function process($p)
    {
        return $p;
    }
}

class AAA
{
    public function func()
    {
        $b = new BBB;
        $c = new CCC;
        $i = 34;
        $item = array('foo' => 'bar');
        try {
            $c->process($b->xyz($item['foo'], $i));
        }
        catch(ABCException $e) {
            $b->xyz($item['foo'], $i);
        }
    } // end func();
}

class Runner
{
    public function run($x)
    {
        try {
            $x->func();
        }
        catch(ABCException $e) {
            throw new Exception;
        }
    }
}

try {
    $runner = new Runner;
    $runner->run(new AAA);
}
catch(Exception $e) {
    die('Exception thrown');
}

?>
--EXPECT--
Exception thrown

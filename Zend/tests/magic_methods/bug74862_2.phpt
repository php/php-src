--TEST--
Bug #74862 (Unable to clone instance when private __clone defined in a child class)
--FILE--
<?php

class main {
}

class a extends main {
    private function __clone()
    {

    }

    private function __construct()
    {

    }

    public static function getInstance()
    {
        return new static();
    }

    public function cloneIt()
    {
        $a = clone $this;

        return $a;
    }
}

class c extends a {

}

// private constructor
$d = c::getInstance();

// private clone
$e = $d->cloneIt();
var_dump($e);
?>
--EXPECT--
object(c)#2 (0) {
}

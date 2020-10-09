--TEST--
Bug #62500 (Segfault in DateInterval class when extended)
--INI--
date.timezone=GMT
--FILE--
<?php
class Crasher extends DateInterval {
    public $foo;
    public function __construct($time_spec) {
        var_dump($this->foo);
        $this->foo = 3;
        var_dump($this->foo);
        var_dump($this->{2});
        parent::__construct($time_spec);
    }
}
try {
    $c = new Crasher('blah');
} catch (Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECTF--
NULL
int(3)

Warning: Undefined property: Crasher::$2 in %s on line %d
NULL
string(%s) "DateInterval::__construct(): Unknown or bad format (blah)"

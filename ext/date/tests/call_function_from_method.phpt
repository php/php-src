--TEST--
Call to date function from a method and call to date method from call_user_func
--INI--
date.timezone=UTC
--FILE--
<?php

class Date {
    public function __construct($in) {
        $this->date = date_create($in);
    }

    public function getYear1() {
        return date_format($this->date, 'Y');
    }

    public function getYear2() {
        return call_user_func([$this->date, 'format'], 'Y');
    }
}

$d = new Date('NOW');
var_dump($d->getYear1());
var_dump($d->getYear2());

?>
--EXPECTF--
string(4) "%d"
string(4) "%d"

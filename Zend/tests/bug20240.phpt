--TEST--
Bug #20240 (order of destructor calls)
--FILE--
<?php

class test
{
    public $member;

    function __construct() {
        $this->member = 1;
        register_shutdown_function(array($this, 'destructor'));
    }

    function destructor() {
        print __METHOD__ . "\n";
    }

    function __destruct() {
        print __METHOD__ . "\n";
    }

    function add() {
        $this->member += 1;
        print $this->member."\n";
    }
}

$t = new test();

$t->add();
$t->add();

echo "Done\n";
?>
--EXPECT--
2
3
Done
test::destructor
test::__destruct

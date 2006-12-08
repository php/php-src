--TEST--
Bug #38146 (Cannot use array returned from foo::__get('bar') in write context)
--FILE--
<?php
class foo {
    public function __get($member) {
        $f = array("foo"=>"bar","bar"=>"foo");
        return $f;
    }
}

$f = new foo();
foreach($f->bar as $key => $value) {
    print "$key => $value\n";
}
?>
--EXPECTF--
Notice: Indirect modification of overloaded property foo::$bar has no effect in %sbug38146.php on line 10
foo => bar
bar => foo

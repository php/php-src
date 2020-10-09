--TEST--
preg_replace_callback_array() basic functions
--FILE--
<?php

class Rep {
    public function __invoke() {
        return "d";
    }
}

class Foo {
    public static function rep($rep) {
        return "ok";
    }
}

function b() {
    return "b";
}

var_dump(preg_replace_callback_array(
    array(
        "/a/" => 'b',
        "/b/" => function () { return "c"; },
        "/c/" => new Rep,
        '/d/' => array("Foo", "rep")), 'a'));

var_dump(preg_replace_callback_array(
    array(
        "/a/" => 'b',
        "/c/" => new Rep,
        "/b/" => function () { return "ok"; },
        '/d/' => array("Foo", "rep")), 'a'));

var_dump(preg_replace_callback_array(
    array(
        '/d/' => array("Foo", "rep"),
        "/c/" => new Rep,
        "/a/" => 'b',
        "/b/" => function($a) { return "ok"; }), 'a', -1, $count));
var_dump($count);

var_dump(preg_replace_callback_array(
    array('/a/' => 'b', "/c/" => new Rep),
    array('a', 'c')));

?>
--EXPECT--
string(2) "ok"
string(2) "ok"
string(2) "ok"
int(2)
array(2) {
  [0]=>
  string(1) "b"
  [1]=>
  string(1) "d"
}

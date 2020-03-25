--TEST--
Bug #37138 (autoloader tries to load callback'ed self and parent)
--FILE--
<?php
spl_autoload_register(function ($CN) { var_dump ($CN); });
class st {
    public static function e () {echo ("EHLO\n");}
    public static function e2 () {call_user_func (array ('self', 'e'));}
}
class stch extends st {
    public static function g () {call_user_func (array ('parent', 'e'));}
}
st::e ();
st::e2 ();
stch::g ();
?>
--EXPECT--
EHLO
EHLO
EHLO

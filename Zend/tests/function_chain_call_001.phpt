--TEST--
function chained call
--FILE--
<?php
class Invokeable {
   public function __invoke() {
        var_dump(__METHOD__);
   }
}
function a() {
   var_dump(__FUNCTION__);
   return "b";
}

function b() {
   var_dump(__FUNCTION__);
   return function () {
       var_dump(__FUNCTION__);
       return new Invokeable();
   };
}

(((a())
       ()) // calling b
          ()) // calling closure
             (); // calling invokeable
--EXPECTF--
string(1) "a"
string(1) "b"
string(9) "{closure}"
string(20) "Invokeable::__invoke"

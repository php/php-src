--TEST--
Closure 046: Closure instance call
--FILE--
<?php

(function () {
   var_dump(__FUNCTION__);
})();

((function() {
  var_dump($this);
})->bindTo(new Stdclass()))();

((function() {
    return function() {
        var_dump(123);
    };
})())();

?>
--EXPECTF--
string(9) "{closure}"
object(stdClass)#3 (0) {
}
int(123)

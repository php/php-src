--TEST--
compact() with object context
--FILE--
<?php

var_dump(
    (new class {
        function test(){
            return compact('this');
        }
    })->test()
);

var_dump(
    (new class {
        function test(){
            return compact([['this']]);
        }
    })->test()
);

var_dump(
    (new class {
        function test(){
            return (function(){ return compact('this'); })();
        }
    })->test()
);

?>
--EXPECTF--
array(1) {
  ["this"]=>
  object(class@anonymous%0%s:4$%x)#1 (0) {
  }
}
array(1) {
  ["this"]=>
  object(class@anonymous%0%s:12$%x)#1 (0) {
  }
}
array(1) {
  ["this"]=>
  object(class@anonymous%0%s:20$%x)#1 (0) {
  }
}

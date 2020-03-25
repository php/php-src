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
--EXPECT--
array(1) {
  ["this"]=>
  object(class@anonymous)#1 (0) {
  }
}
array(1) {
  ["this"]=>
  object(class@anonymous)#1 (0) {
  }
}
array(1) {
  ["this"]=>
  object(class@anonymous)#1 (0) {
  }
}

--TEST--
Testing lambda function in set_exception_handler()
--FILE--
<?php
function au($class) {
        eval('class handler {
                  function handle($e) {
                      echo $e->getMessage()."\n";
                  }
              }');
}

function __autoload($class) {
        au($class);
}

//spl_autoload_register('au');

set_exception_handler(function($exception) {
        $h = new handler();
        $h->handle($exception);
});

throw new Exception('exception');
?>
--EXPECT--
exception

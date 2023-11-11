--TEST--
Bug #80802: zend_jit_fetch_indirect_var assert failure with tracing JIT
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=tracing
--EXTENSIONS--
opcache
--FILE--
<?php
abstract class AsyncTask{
        private static $threadLocalStorage = null;

        protected function storeLocal(string $key, $complexData) : void{
                if(self::$threadLocalStorage === null){
                        self::$threadLocalStorage = new \ArrayObject();
                }
                self::$threadLocalStorage[spl_object_id($this)][$key] = $complexData;
        }

        final public function __destruct(){
                $this->reallyDestruct();
                if(self::$threadLocalStorage !== null and isset(self::$threadLocalStorage[$h = spl_object_id($this)])){
                        unset(self::$threadLocalStorage[$h]);
                        if(self::$threadLocalStorage->count() === 0){
                                self::$threadLocalStorage = null;
                        }
                }
        }

        protected function reallyDestruct() : void{

        }
}

class Task extends AsyncTask{
        public function __construct(){
                $this->storeLocal("thing1", new stdClass);
        }
}

for($i = 0; $i < 10000; ++$i){
        new Task;
}
echo "OK\n";
?>
--EXPECT--
OK

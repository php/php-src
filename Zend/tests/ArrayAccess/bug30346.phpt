--TEST--
Bug #30346 (arrayAccess and using $this)
--FILE--
<?php

class Test implements ArrayAccess
{
        public function __construct() { }
        public function offsetExists( $offset ): bool { return false; }
        public function offsetGet( $offset ): mixed { return $offset; }
        public function offsetSet( $offset, $data ): void { }
        public function offsetUnset( $offset ): void { }
}

$post = new Test;
$id = 'page';
echo $post[$id.'_show'];
echo "\n";

?>
--EXPECT--
page_show

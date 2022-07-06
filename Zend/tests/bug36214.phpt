--TEST--
Bug #36214 (__get method works properly only when conditional operator is used)
--FILE--
<?php
class context {
  public $stack = array();

  public function __set($name,$var) {
    $this->stack[$name] = $var;return;
  }

  public function &__get($name) {
    return $this->stack[$name];
  }
}

$ctx = new context;
$ctx->comment_preview = array();
$ctx->comment_preview[0] = 1;
$ctx->comment_preview[1] = 2;
var_dump($ctx->comment_preview);

$comment_preview = array();
$comment_preview[0] = 1;
$comment_preview[1] = 2;
$ctx->comment_preview = $comment_preview;
var_dump($ctx->comment_preview);

$ctx->comment_preview = new ArrayObject();
$ctx->comment_preview[0] = 1;
$ctx->comment_preview[1] = 2;
var_dump($ctx->comment_preview);
?>
--EXPECTF--
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

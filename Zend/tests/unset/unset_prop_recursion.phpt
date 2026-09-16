--TEST--
Unset property where unset will recursively access property again
--FILE--
<?php
class Node {
    public $parent = null;
    public $children = [];
    function insert(Node $node) {
        $node->parent = $this;
        $this->children[] = $node;
    }
    function __destruct() {
        var_dump($this);
        unset($this->children);
    }
}

$a = new Node;
$a->insert(new Node);
$a->insert(new Node);
?>
--EXPECT--
object(Node)#1 (2) {
  ["parent"]=>
  NULL
  ["children"]=>
  array(2) {
    [0]=>
    object(Node)#2 (2) {
      ["parent"]=>
      *RECURSION*
      ["children"]=>
      array(0) {
      }
    }
    [1]=>
    object(Node)#3 (2) {
      ["parent"]=>
      *RECURSION*
      ["children"]=>
      array(0) {
      }
    }
  }
}
object(Node)#2 (2) {
  ["parent"]=>
  object(Node)#1 (2) {
    ["parent"]=>
    NULL
  }
  ["children"]=>
  array(0) {
  }
}
object(Node)#3 (2) {
  ["parent"]=>
  object(Node)#1 (2) {
    ["parent"]=>
    NULL
  }
  ["children"]=>
  array(0) {
  }
}

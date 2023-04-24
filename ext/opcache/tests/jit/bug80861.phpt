--TEST--
Bug #80839: PHP problem with JIT
--INI--
error_log=
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=1M
opcache.jit=tracing
--EXTENSIONS--
opcache
--FILE--
<?php
declare(strict_types=1);

// --------------------------------------------------------------------
class Node
{
    public $column = null;
    public $left = null;
    public $right = null;
    public $up = null;
    public $down = null;

    public static function joinLR(Node $a, Node $b): void
    {
        $a->right = $b;
        $b->left = $a;
    }

    public static function joinDU(Node $a, Node $b): void
    {
        $a->up = $b;
        $b->down = $a;
    }
}

// --------------------------------------------------------------------
class Column extends Node
{
    public function __construct()
    {
        $this->column = $this;
        $this->up = $this;
        $this->down = $this;
    }
}

// --------------------------------------------------------------------
class Matrix
{
    public $head = null;

    public function __construct()
    {
        $this->head = new Node();
        Node::joinLR($this->head, $this->head);
    }

    // $from is array[][] of bool
    public static function fromArray(array $from): Matrix
    {
        $m = new Matrix();
        $rowCount = count($from);
        if ($rowCount == 0) {
            return $m;
        }
        $columnCount = count($from[0]);
        if ($columnCount == 0) {
            return $m;
        }
        // we generate 2D double linked circular list of nodes from the input 2D bool array
        // might be not relevant for the bug
        $c = new Column();
        Node::joinLR($m->head, $c);
        for ($j = 1; $j < $columnCount; $j++) {
            $nextCol = new Column();
            Node::joinLR($c, $nextCol);
            $c = $c->right;
        }
        Node::joinLR($c, $m->head);
        $c = $m->head->right;
        error_log("These are the array bounds: $rowCount * $columnCount");
        for ($j = 0; $j < $columnCount; $j++) {
            $prev = $c;
            for ($i = 0; $i < $rowCount; $i++) {
                // next line generates the warnings despite $i and $j is within bounds
                if ($from[$i][$j]) {
                    $node = new Node();
                    $node->column = $c;
                    Node::joinDU($node, $prev);
                    Node::joinLR($node, $node);
                    $prev = $node;
                    // ... code to generate $m excluded
                }
            }
            Node::joinDU($c, $prev);
            $c = $c->right;
        }
        return $m;
    }
}

// --------------------------------------------------------------------
// simple driver code - fills up a 2D bool matrix and calls the static matrix constructing function above
for ($y = 0; $y < 10; $y++) {
    for ($x = 0; $x < 10; $x++) {
        $a[$y][$x] = true;
    }
}
$m = Matrix::fromArray($a);
--EXPECT--
These are the array bounds: 10 * 10

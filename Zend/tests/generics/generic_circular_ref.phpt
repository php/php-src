--TEST--
Generic class: circular references with GC
--FILE--
<?php

class Node<T> {
    public T $value;
    public ?self $next = null;

    public function __construct(T $value) {
        $this->value = $value;
    }
}

// 1. Simple circular reference
$a = new Node<int>(1);
$b = new Node<int>(2);
$a->next = $b;
$b->next = $a; // circular

echo "1. a=" . $a->value . ", b=" . $b->value . "\n";
echo "1. a->next=" . $a->next->value . "\n";
echo "1. b->next=" . $b->next->value . "\n";

// Break and let GC handle it
unset($a, $b);
gc_collect_cycles();
echo "1. GC OK\n";

// 2. Self-referential
$self = new Node<int>(42);
$self->next = $self;
echo "2. self=" . $self->value . "\n";
echo "2. self->next=" . $self->next->value . "\n";
unset($self);
gc_collect_cycles();
echo "2. GC OK\n";

// 3. Longer cycle
$n1 = new Node<string>("a");
$n2 = new Node<string>("b");
$n3 = new Node<string>("c");
$n1->next = $n2;
$n2->next = $n3;
$n3->next = $n1; // cycle

echo "3. chain: " . $n1->value . "->" . $n2->value . "->" . $n3->value . "\n";
unset($n1, $n2, $n3);
gc_collect_cycles();
echo "3. GC OK\n";

// 4. Many cycles to stress GC
for ($i = 0; $i < 100; $i++) {
    $x = new Node<int>($i);
    $y = new Node<int>($i + 1);
    $x->next = $y;
    $y->next = $x;
}
gc_collect_cycles();
echo "4. 100 cycles GC OK\n";

echo "Done.\n";
?>
--EXPECT--
1. a=1, b=2
1. a->next=2
1. b->next=1
1. GC OK
2. self=42
2. self->next=42
2. GC OK
3. chain: a->b->c
3. GC OK
4. 100 cycles GC OK
Done.

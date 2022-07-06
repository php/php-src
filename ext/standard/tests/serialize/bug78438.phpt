--TEST--
Bug #78438 (Corruption when __unserializing deeply nested structures)
--FILE--
<?php

class Node
{
    public $childs = [];

    public function __serialize()
    {
        return [$this->childs];
    }

    public function __unserialize(array $data)
    {
        list($this->childs) = $data;
    }
}

function createTree ($width, $depth) {
    $root = new Node();

    $nextLevel = [$root];

    for ($level=1; $level<$depth; $level++) {
        $levelRoots = $nextLevel;
        $nextLevel = [];

        while (count($levelRoots) > 0) {
            $levelRoot = array_shift($levelRoots);

            for ($w = 0; $w < $width; $w++) {
                $tester = new Node();

                $levelRoot->childs[] = $tester;

                $nextLevel[] = $tester;
            }
        }
    }

    return $root;
}


$width = 3;
ob_implicit_flush();

foreach (range(1, 8) as $depth) {
    $tree = createTree($width, $depth);

    echo "Testcase tree $width x $depth".PHP_EOL;

    echo "> Serializing now".PHP_EOL;
    $serialized = serialize($tree);

    echo "> Unserializing now".PHP_EOL;
    $tree = unserialize($serialized);

    // Lets test whether all is ok!
    $expectedSize = ($width**$depth - 1)/($width-1);

    $nodes = [$tree];
    $count = 0;

    while (count($nodes) > 0) {
        $count++;

        $node = array_shift($nodes);
        foreach ($node->childs as $node) {
            $nodes[] = $node;
        }
    }

    echo "> Unserialized total node count was $count, expected $expectedSize: ".($expectedSize === $count ? 'CORRECT!' : 'INCORRECT');

    echo PHP_EOL;
    echo PHP_EOL;
}
?>
--EXPECT--
Testcase tree 3 x 1
> Serializing now
> Unserializing now
> Unserialized total node count was 1, expected 1: CORRECT!

Testcase tree 3 x 2
> Serializing now
> Unserializing now
> Unserialized total node count was 4, expected 4: CORRECT!

Testcase tree 3 x 3
> Serializing now
> Unserializing now
> Unserialized total node count was 13, expected 13: CORRECT!

Testcase tree 3 x 4
> Serializing now
> Unserializing now
> Unserialized total node count was 40, expected 40: CORRECT!

Testcase tree 3 x 5
> Serializing now
> Unserializing now
> Unserialized total node count was 121, expected 121: CORRECT!

Testcase tree 3 x 6
> Serializing now
> Unserializing now
> Unserialized total node count was 364, expected 364: CORRECT!

Testcase tree 3 x 7
> Serializing now
> Unserializing now
> Unserialized total node count was 1093, expected 1093: CORRECT!

Testcase tree 3 x 8
> Serializing now
> Unserializing now
> Unserialized total node count was 3280, expected 3280: CORRECT!

--TEST--
Bug #65328 (Segfault when getting SplStack object Value)
--FILE--
<?php
/**
 * @author AlexanderC
 */

class Tree
{
    /**
     * @var Node
     */
    protected $head;

    /**
     * @param Node $head
     */
    public function __construct(Node $head = null)
    {
        $this->head = $head ? : new Node('HEAD');
    }

    /**
     * @return Node
     */
    public function getHead()
    {
        return $this->head;
    }

    /**
     * @param mixed $uid
     * @return Node|bool
     */
    public function find($uid)
    {
        $iterator = $this->getIterator();

        /** @var Node $node */
        foreach($iterator as $node) {
            if($node->getUid() === $uid) {
                return $node;
            }
        }

        return false;
    }

    /**
     * @param mixed $uid
     * @return \SplStack
     */
    public function & findAll($uid)
    {
        $result = new \SplStack();

        /** @var Node $node */
        foreach($this->getIterator() as $node) {
            if($node->getUid() == $uid) {
                $result->push($node);
            }
        }

        return $result;
    }

    /**
     * @return \RecursiveIteratorIterator
     */
    public function getIterator()
    {
        return new \RecursiveIteratorIterator(
            $this->head->getChildren(),
            \RecursiveIteratorIterator::SELF_FIRST
        );
    }
}

class Node extends \RecursiveArrayIterator implements \Countable
{
    /**
     * @var array
     */
    protected $children = [];

    /**
     * @var Node
     */
    protected $parent;

    /**
     * @var mixed
     */
    protected $data;

    /**
     * @var mixed
     */
    protected $uid;

    /**
     * @var int
     */
    protected $index = 0;

    /**
     * @var bool
     */
    protected $assureUnique;

    /**
     * @param mixed $data
     * @param mixed $uid
     * @param Node $parent
     * @param bool $assureUnique
     */
    public function __construct($data, $uid = null, Node $parent = null, $assureUnique = false)
    {
        if(null !== $parent) {
            $this->parent = $parent;
        }

        $this->data = $data;
        $this->uid = $uid ? : uniqid(sha1(serialize($data)), true);
        $this->assureUnique = $assureUnique;
    }

    /**
     * @param mixed $uid
     */
    public function setUid($uid)
    {
        $this->uid = $uid;
    }

    /**
     * @return mixed
     */
    public function getUid()
    {
        return $this->uid;
    }

    /**
     * @param Node $child
     */
    public function addChild(Node $child)
    {
        $child->setParent($this);
        $this->children[] = $child;
    }

    /**
     * @param array $children
     */
    public function setChildren(array $children)
    {
        $this->children = $children;
    }

    /**
     * @return array
     */
    public function getChildrenArray()
    {
        return $this->children;
    }

    /**
     * @param mixed $data
     */
    public function setData($data)
    {
        $this->data = $data;
    }

    /**
     * @return mixed
     */
    public function getData()
    {
        return $this->data;
    }

    /**
     * @param Node $parent
     * @throws \RuntimeException
     */
    public function setParent(Node $parent)
    {
        if(true === $this->assureUnique && !self::checkUnique($parent, $this->uid)) {
            throw new \RuntimeException("Node uid is not unique in assigned node tree");
        }

        $this->parent = $parent;
    }

    /**
     * @param Node $node
     * @param mixed $uid
     * @return bool
     */
    protected static function checkUnique(Node $node, $uid)
    {
        $headNode = $node;
        do {
            $headNode = $node;
        } while($node = $node->getParent());

        $tree = new Tree($headNode);

        return !$tree->find($uid);
    }

    /**
     * @return \IJsonRPC\Helpers\Tree\Node
     */
    public function getParent()
    {
        return $this->parent;
    }

    /**
     * @return Node
     */
    public function current()
    {
        return $this->children[$this->index];
    }

    /**
     * @return scalar
     */
    public function key()
    {
        return $this->index;
    }

    /**
     * @return void
     */
    public function next()
    {
        ++$this->index;
    }

    /**
     * @return void
     */
    public function rewind()
    {
        $this->index = 0;
    }

    /**
     * @return bool
     */
    public function valid()
    {
        return array_key_exists($this->index, $this->children);
    }

    /**
     * @return int
     */
    public function count()
    {
        return count($this->children);
    }

    /**
     * @return bool
     */
    public function hasChildren()
    {
        return !empty($this->children);
    }

    /**
     * @return \RecursiveArrayIterator
     */
    public function getChildren()
    {
        return new \RecursiveArrayIterator($this->children);
    }
}

$tree = new Tree();
$node1 = new Node('value1', 1);
$tree->getHead()->addChild($node1);
$node2 = new Node('value2', 2);
$node1->addChild($node2);

print_r($tree->findAll(2)->offsetGet(0));
--EXPECTF--
Node Object
(
    [children:protected] => Array
        (
        )

    [parent:protected] => Node Object
        (
            [children:protected] => Array
                (
                    [0] => Node Object
 *RECURSION*
                )

            [parent:protected] => Node Object
                (
                    [children:protected] => Array
                        (
                            [0] => Node Object
 *RECURSION*
                        )

                    [parent:protected] => 
                    [data:protected] => HEAD
                    [uid:protected] => %s
                    [index:protected] => 0
                    [assureUnique:protected] => 
                    [storage:ArrayIterator:private] => Array
                        (
                        )

                )

            [data:protected] => value1
            [uid:protected] => 1
            [index:protected] => 1
            [assureUnique:protected] => 
            [storage:ArrayIterator:private] => Array
                (
                )

        )

    [data:protected] => value2
    [uid:protected] => 2
    [index:protected] => 0
    [assureUnique:protected] => 
    [storage:ArrayIterator:private] => Array
        (
        )

)

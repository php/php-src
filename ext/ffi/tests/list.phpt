--TEST--
FFI Double linked lists
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
class DList {
	private static $ffi = null;
	private $root;

	function __construct() {
		if (is_null(self::$ffi)) {
			self::$ffi =
				FFI::cdef("
					typedef struct _dlist dlist;
					struct _dlist {
						int data;
						dlist *prev;
						dlist *next;
					};
				");
		}
		$node = FFI::addr(self::$ffi->new("dlist", false));
		$node->data = 0;
		$node->next = $node;
		$node->prev = $node;
		$this->root = $node;
	}

	function __destruct() {
		$root = $this->root;
		$node = $root->next;
		while ($node != $root) {
			$prev = $node;
			$node = $node->next;
			FFI::free($prev);
		}
		FFI::free($root);
	}

	function add(int $data) {
		$node = FFI::addr(self::$ffi->new("dlist", false));
		$node->data = $data;
		$node->next = $this->root;
		$node->prev = $this->root->prev;
		$this->root->prev->next = $node;
		$this->root->prev = $node;
	}

	function del(int $data) {
		$root = $this->root;
		$node = $root->next;
		while ($node != $root) {
			if ($node->data == $data) {
				$node->prev->next = $node->next;
				$node->next->prev = $node->prev;
				FFI::free($node);
				break;
			}
			$node = $node->next;
		}
	}

	function print() {
		echo "[";
		$first = true;
		$root = $this->root;
		$node = $root->next;
		while ($node != $root) {
			if (!$first) {
				echo ", ";
			} else {
				$first = false;
			}
			echo $node->data;
			$node = $node->next;
		}
		echo "]\n";
	}
}

$dlist = new Dlist;
$dlist->add(1);
$dlist->add(3);
$dlist->add(5);
$dlist->print();
$dlist->del(3);
$dlist->print();
echo "OK\n";
--EXPECT--
[1, 3, 5]
[1, 5]
OK

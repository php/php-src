--TEST--
SPL: SplObjectStorage
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

class MyObjectStorage extends SplObjectStorage
{
	function rewind()
	{
		echo __METHOD__ . "()\n";
		parent::rewind();
	}

	function valid()
	{
		echo __METHOD__ . "(" . (parent::valid() ? 1 : 0) . ")\n";
		return parent::valid();
	}

	function key()
	{
		echo __METHOD__ . "(" . parent::key() . ")\n";
		return parent::key();
	}

	function current()
	{
		echo __METHOD__ . "(" . parent::current()->getName() . ")\n";
		return parent::current();
	}

	function next()
	{
		echo __METHOD__ . "()\n";
		parent::next();
	}
}

class ObserverImpl implements SplObserver
{
	protected $name = '';

	function __construct($name = 'obj')
	{
		$this->name = '$' . $name;
	}

	function update(SplSubject $subject)
	{
		echo $this->name . '->' . __METHOD__ . '(' . $subject->getName() . ");\n";
	}
	
	function getName()
	{
		return $this->name;
	}
}

class SubjectImpl implements SplSubject
{
	protected $name = '';
	protected $observers;

	function __construct($name = 'sub')
	{
		$this->observers = new MyObjectStorage;
		$this->name = '$' . $name;
	}

	function attach(SplObserver $observer)
	{
		echo $this->name . '->' . __METHOD__ . '(' . $observer->getName() . ");\n";
		$this->observers->attach($observer);
	}
	
	function detach(SplObserver $observer)
	{
		echo $this->name . '->' . __METHOD__ . '(' . $observer->getName() . ");\n";
		$this->observers->detach($observer);
	}
	
	function count()
	{
		return $this->observers->count();
	}
	
	function notify()
	{
		echo $this->name . '->' . __METHOD__ . "();\n";
		foreach($this->observers as $key => $observer)
		{
			$observer->update($this);
		}
	}

	function getName()
	{
		return $this->name;
	}
	
	function contains($obj)
	{
		return $this->observers->contains($obj);
	}
}

$sub = new SubjectImpl;

$ob1 = new ObserverImpl("ob1");
$ob2 = new ObserverImpl("ob2");
$ob3 = new ObserverImpl("ob3");

var_dump($sub->contains($ob1));
$sub->attach($ob1);
var_dump($sub->contains($ob1));
$sub->attach($ob1);
$sub->attach($ob2);
$sub->attach($ob3);
var_dump($sub->count());

$sub->notify();

$sub->detach($ob3);
var_dump($sub->count());

$sub->notify();

$sub->detach($ob2);
$sub->detach($ob1);
var_dump($sub->count());

$sub->notify();

$sub->attach($ob3);
var_dump($sub->count());

$sub->notify();

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
bool(false)
$sub->SubjectImpl::attach($ob1);
bool(true)
$sub->SubjectImpl::attach($ob1);
$sub->SubjectImpl::attach($ob2);
$sub->SubjectImpl::attach($ob3);
int(3)
$sub->SubjectImpl::notify();
MyObjectStorage::rewind()
MyObjectStorage::valid(1)
MyObjectStorage::current($ob1)
MyObjectStorage::key(0)
$ob1->ObserverImpl::update($sub);
MyObjectStorage::next()
MyObjectStorage::valid(1)
MyObjectStorage::current($ob2)
MyObjectStorage::key(1)
$ob2->ObserverImpl::update($sub);
MyObjectStorage::next()
MyObjectStorage::valid(1)
MyObjectStorage::current($ob3)
MyObjectStorage::key(2)
$ob3->ObserverImpl::update($sub);
MyObjectStorage::next()
MyObjectStorage::valid(0)
$sub->SubjectImpl::detach($ob3);
int(2)
$sub->SubjectImpl::notify();
MyObjectStorage::rewind()
MyObjectStorage::valid(1)
MyObjectStorage::current($ob1)
MyObjectStorage::key(0)
$ob1->ObserverImpl::update($sub);
MyObjectStorage::next()
MyObjectStorage::valid(1)
MyObjectStorage::current($ob2)
MyObjectStorage::key(1)
$ob2->ObserverImpl::update($sub);
MyObjectStorage::next()
MyObjectStorage::valid(0)
$sub->SubjectImpl::detach($ob2);
$sub->SubjectImpl::detach($ob1);
int(0)
$sub->SubjectImpl::notify();
MyObjectStorage::rewind()
MyObjectStorage::valid(0)
$sub->SubjectImpl::attach($ob3);
int(1)
$sub->SubjectImpl::notify();
MyObjectStorage::rewind()
MyObjectStorage::valid(1)
MyObjectStorage::current($ob3)
MyObjectStorage::key(0)
$ob3->ObserverImpl::update($sub);
MyObjectStorage::next()
MyObjectStorage::valid(0)
===DONE===

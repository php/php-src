--TEST--
Bug #38618 (RecursiveArrayIterator::hasChildren() follows objects)
--FILE--
<?php

class FruitPublic
{
  public $title;

  public function __construct($title)
  {
    $this->title = $title;
  }

  public function __toString()
  {
    return $this->title;
  }
}

class FruitProtected
{
  protected $title;

  public function __construct($title)
  {
    $this->title = $title;
  }

  public function __toString()
  {
    return $this->title;
  }
}

function test_array($array, $which, $flags = 0)
{
  echo "===$which===\n";
  $it = new RecursiveArrayIterator($array, $flags);
  foreach (new RecursiveIteratorIterator($it) as $k => $fruit) {
    echo $k , ' => ', $fruit, "\n";
  }
}

$array = array(
  1 => array(
    1 => array(
      1 => 'apple',
    ),
    2 => array(
      1 => 'grape',
    ),
  ),
);

test_array($array, 'Default with array');

$array = array(
  1 => array(
    1 => array(
      1 => new FruitPublic('apple'),
    ),
    2 => array(
      1 => new FruitPublic('grape'),
    ),
  ),
);

test_array($array, 'Public Property');

$array = array(
  1 => array(
    1 => array(
      1 => new FruitProtected('apple'),
    ),
    2 => array(
      1 => new FruitProtected('grape'),
    ),
  ),
);

test_array($array, 'Protected Property');

test_array($array, 'Public Property New', RecursiveArrayIterator::CHILD_ARRAYS_ONLY);
test_array($array, 'Protected Property New', RecursiveArrayIterator::CHILD_ARRAYS_ONLY);
?>
--EXPECT--
===Default with array===
1 => apple
1 => grape
===Public Property===
title => apple
title => grape
===Protected Property===
===Public Property New===
1 => apple
1 => grape
===Protected Property New===
1 => apple
1 => grape

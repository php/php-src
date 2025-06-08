--TEST--
Readonly classes can be constructed via reflection by ORM
--FILE--
<?php

interface DbConnection {
   public function loadCategory(string $id): Category;
}

class Category {
   public function __construct(public string $name) {}
}

class MockDbConnection implements DbConnection {
   public function loadCategory(string $id): Category {
       echo "hit database\n";
       return new Category("Category {$id}");
   }
}

readonly class Product
{
    public function __construct(
        public string $name,
        public float $price,
        public Category $category,
    ) {}
}

readonly class LazyProduct extends Product
{
   private DbConnection $dbApi;

   private string $categoryId;

   public Category $category {
       get {
           return $this->category ??= $this->dbApi->loadCategory($this->categoryId);
       }
   }
}

$reflect = new ReflectionClass(LazyProduct::class);
$product = $reflect->newInstanceWithoutConstructor();

$nameProperty = $reflect->getProperty('name');
$nameProperty->setAccessible(true);
$nameProperty->setValue($product, 'Iced Chocolate');

$priceProperty = $reflect->getProperty('price');
$priceProperty->setAccessible(true);
$priceProperty->setValue($product, 1.99);

$db = $reflect->getProperty('dbApi');
$db->setAccessible(true);
$db->setValue($product, new MockDbConnection());

$categoryId = $reflect->getProperty('categoryId');
$categoryId->setAccessible(true);
$categoryId->setValue($product, '42');

// lazy loading, hit db
$category1 = $product->category;
echo $category1->name . "\n";

// cached category returned
$category2 = $product->category;
echo $category2->name . "\n";

// same category instance returned
var_dump($category1 === $category2);

// can't be wrong, huh?
var_dump($product);

// cannot set twice
try {
    $categoryId->setValue($product, '420');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
hit database
Category 42
Category 42
bool(true)
object(LazyProduct)#2 (5) {
  ["name"]=>
  string(14) "Iced Chocolate"
  ["price"]=>
  float(1.99)
  ["category"]=>
  object(Category)#8 (1) {
    ["name"]=>
    string(11) "Category 42"
  }
  ["dbApi":"LazyProduct":private]=>
  object(MockDbConnection)#6 (0) {
  }
  ["categoryId":"LazyProduct":private]=>
  string(2) "42"
}
Cannot modify readonly property LazyProduct::$categoryId

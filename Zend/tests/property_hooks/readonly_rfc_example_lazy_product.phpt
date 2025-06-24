--TEST--
Readonly classes can be constructed via reflection by ORM
--FILE--
<?php

class Category {
   public function __construct(public string $id) {}
}

readonly class Product
{
    public function __construct(
        public Category $category,
    ) {}
}

readonly class LazyProduct extends Product
{
   private string $categoryId;

   public Category $category {
       get {
           return $this->category ??= new Category($this->categoryId);
       }
   }
}

$reflect = new ReflectionClass(LazyProduct::class);
$product = $reflect->newInstanceWithoutConstructor();

$categoryId = $reflect->getProperty('categoryId');
$categoryId->setAccessible(true);
$categoryId->setValue($product, '42');

$category1 = $product->category;
$category2 = $product->category;

echo $category1->id . "\n";
echo $category2->id . "\n";

var_dump($category1 === $category2);

// cannot set twice
try {
    $categoryId->setValue($product, '420');
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
42
42
bool(true)
Error: Cannot modify readonly property LazyProduct::$categoryId

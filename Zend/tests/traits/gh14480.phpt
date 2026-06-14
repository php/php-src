--TEST--
GH-14480: Method visibility issue
--FILE--
<?php
trait PropertyHelperTrait
{
    protected function splitPropertyParts(): void
    {
	    echo "OK\n";
    }
}

trait OrmPropertyHelperTrait
{
    abstract protected function splitPropertyParts(): void;
    
    protected function addJoinsForNestedProperty(): void
    {
        $this->splitPropertyParts();
    }
}

trait SearchFilterTrait
{
    use PropertyHelperTrait;
}

abstract class AbstractFilter
{
    use OrmPropertyHelperTrait, PropertyHelperTrait;
    
    public function apply(): void
    {
        $this->filterProperty();
    }
    
    abstract protected function filterProperty(): void;
}

class SearchFilter extends AbstractFilter
{
    use SearchFilterTrait;
    protected function filterProperty(): void
    {
        $this->addJoinsForNestedProperty();
    }
}

class FilterExtension
{
    public function applyToCollection(): void
    {
        (new SearchFilter())->apply();
    }
}

(new FilterExtension)->applyToCollection();
?>
--EXPECT--
OK

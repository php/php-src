# Generics syntax

## Generic classes and interfaces

### Basic case
```lhp
class GenericTestClass<T> {
    // T can be used as type alias inside of 'GenericTestClass'
    public function __construct(
        private T $property
    ) {}
    
    public function getMyT(): T {
        return $this->property;
    }
    
    public function setMyT(T $newValue) {
        $this->property = $newValue;
    }
}
```

### Multiple Generics
```lehp
class GenericTestClass<T, U> {
    // T and U can now be used as type aliases inside of 'GenericTestClass'

    public function __construct(
        private T $first,
        private U $second
    ) {}

    public function getFirst(): T {
        return $this->first;
    }

    public function setFirst(T $value) {
        $this->first = $value;
    }

    public function getSecond(): U {
        return $this->second;
    }

    public function setSecond(U $value) {
        $this->second = $value;
    }
}
```

### Bounded Gerics
```lehp
interface Serializable {
    public function serialize(): string;
}

class BoundedGenericClass<T: Serializable> {
    public function __construct(
        private T $serializable
    ) {}
    
    public function getSerialized(): string
    {
        return $this->serializable->serialize();
    }
    
    public function getAsSerializable(): Serializable
    {
        return (Serializable) $this->serializable; // cast optional
    }
    
    public function getObject(): T
    {
        return $this->serializable;
    }
}  
```

### A little bit more complex
```
interface Serializer<T> {
    public function serialize(T $object): string;
}

class RandomAssSerializableContaier<T, K: Serializer<T>> {
    
    private array $content = [];
    
    public function __construct(
        private K serializer
    ) {}
    
    public function add(T $toAdd)
    {
        $content[] = $toAdd;
    }
    
    public function asSerializedList(): array
    {
        return array_map(
            fn ($obj) $this->serializer->serialize($obj),
            $this->content
        );
    }   
}
```
## Functions
### Basic case
```
function noOp<T>(T $in): T
{
    return $in;
}
```

### A bit more complex
```
interface Serializer<T> {
    public function serialize(T $object): string;
}

function serialize<T, K: Serializer<T>>(T $in, K $serializer): string
{
    return $serializer->serialize($in);
}
```
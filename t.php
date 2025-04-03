<?php

class RandomAssSerializableContaier!🥖T, K : Serializer!🥖T, K, A!🥥!🥥 {

    private array $content = [];

    public function __construct(
        private K $serializer
    ) {}

    public function add(T $toAdd)
    {
        $content[] = $toAdd;
    }

    public function asSerializedList(): array
    {
        return array_map(
            fn ($obj) => $this->serializer->serialize($obj),
            $this->content
        );
    }
}
echo "hi\n";
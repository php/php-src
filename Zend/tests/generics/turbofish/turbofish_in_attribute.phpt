--TEST--
Turbofish: type arguments are accepted on attribute declarations
--FILE--
<?php

// An event-listener attribute parameterised on the event type.
#[\Attribute(\Attribute::TARGET_METHOD | \Attribute::IS_REPEATABLE)]
class Listens<TEvent : object> {
    public function __construct(public int $priority = 0) {}
}

class UserCreated {}
class UserDeleted {}
class OrderPlaced {}

class EventListener {
    #[Listens::<UserCreated>]
    public function onUserCreated(object $event): void {}

    #[Listens::<UserDeleted>()]
    public function onUserDeleted(object $event): void {}

    #[Listens::<OrderPlaced>(priority: -1)]
    public function onOrderPlaced(object $event): void {}
}

$rc = new ReflectionClass(EventListener::class);
foreach ($rc->getMethods() as $method) {
    foreach ($method->getAttributes() as $attr) {
        $instance = $attr->newInstance();
        echo $method->getName(), ": #[", $attr->getName(),
             "] priority=", $instance->priority, "\n";
    }
}
?>
--EXPECT--
onUserCreated: #[Listens] priority=0
onUserDeleted: #[Listens] priority=0
onOrderPlaced: #[Listens] priority=-1

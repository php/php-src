--TEST--
nested enums
--FILE--
<?php
enum Status {
    case Active;
    case Inactive;

    private enum Processing {
        case Pending;
        case Completed;
    }

    public class Message {

        private Processing $processing = Processing::Pending;

        public function __construct(private Status $status) {}

        public function getMessage(): string {
            return match ([$this->status, $this->processing]) {
                [Status::Active, Processing::Pending] => "Active and Pending",
                [Status::Active, Processing::Completed] => "Active and Completed",
                [Status::Inactive, Processing::Pending] => "Inactive and Pending",
                [Status::Inactive, Processing::Completed] => throw new LogicException('should not happen'),
            };
        }
    }

    public function createMessage(): Message {
        return new Message($this);
    }
}

var_dump(Status::Active->createMessage());
echo Status::Inactive->createMessage()->getMessage() . "\n";
?>
--EXPECT--
object(Status\Message)#3 (2) {
  ["processing":"Status\Message":private]=>
  enum(Status\Processing::Pending)
  ["status":"Status\Message":private]=>
  enum(Status::Active)
}
Inactive and Pending

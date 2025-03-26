--TEST--
ReflectionEnum::__toString() (larger case, unbacked)
--FILE--
<?php

interface MyStringable {
    public function toString(): string;
}

enum Suit implements MyStringable {
    case Hearts;
    case Diamonds;
    case Clubs;
    case Spades;
    
    public const Suit OtherHearts = Suit::Hearts;

    public function toString(): string {
        return $this->name;
    }
}

$r = new ReflectionClass( Suit::class );
echo $r;
echo "\n";
$r = new ReflectionEnum( Suit::class );
echo $r;

var_export( Suit::cases() );

?>
--EXPECTF--
Enum [ <user> enum Suit implements MyStringable, UnitEnum ] {
  @@ %sReflectionEnum_toString_unbacked.php 7-18

  - Enum cases [4] {
    Case Hearts
    Case Diamonds
    Case Clubs
    Case Spades
  }

  - Constants [1] {
    Constant [ public Suit OtherHearts ] { Object }
  }

  - Static properties [0] {
  }

  - Static methods [1] {
    Method [ <internal, prototype UnitEnum> static public method cases ] {

      - Parameters [0] {
      }
      - Return [ array ]
    }
  }

  - Properties [1] {
    Property [ public protected(set) readonly string $name ]
  }

  - Methods [1] {
    Method [ <user, prototype MyStringable> public method toString ] {
      @@ %sReflectionEnum_toString_unbacked.php 15 - 17

      - Parameters [0] {
      }
      - Return [ string ]
    }
  }
}

Enum [ <user> enum Suit implements MyStringable, UnitEnum ] {
  @@ %sReflectionEnum_toString_unbacked.php 7-18

  - Enum cases [4] {
    Case Hearts
    Case Diamonds
    Case Clubs
    Case Spades
  }

  - Constants [1] {
    Constant [ public Suit OtherHearts ] { Object }
  }

  - Static properties [0] {
  }

  - Static methods [1] {
    Method [ <internal, prototype UnitEnum> static public method cases ] {

      - Parameters [0] {
      }
      - Return [ array ]
    }
  }

  - Properties [1] {
    Property [ public protected(set) readonly string $name ]
  }

  - Methods [1] {
    Method [ <user, prototype MyStringable> public method toString ] {
      @@ %sReflectionEnum_toString_unbacked.php 15 - 17

      - Parameters [0] {
      }
      - Return [ string ]
    }
  }
}
array (
  0 => 
  \Suit::Hearts,
  1 => 
  \Suit::Diamonds,
  2 => 
  \Suit::Clubs,
  3 => 
  \Suit::Spades,
)

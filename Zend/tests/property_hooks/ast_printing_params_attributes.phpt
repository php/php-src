--TEST--
Hook parameter list and attributes AST printing
--FILE--
<?php

try {
    assert(false && new class {
        public int $prop1 {
            set(int $value) => $value * 2;
        }
        public $prop2 {
            #[A]
            get => $this->prop2;
            #[A(1), B]
            #[C]
            final set($value) {
                $this->prop2 = $value;
            }
        }
        public string $prop3 {
            set(#[SensitiveParameter] string|int $value) => (string) $value;
        }
        public $prop4 {
            #[A] final &get => $this->prop4;
        }
        public function __construct(
            public int $prop5 {
                #[A]
                set(int $value) => $value + 1;
            },
        ) {}
    });
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
assert(false && new class {
    public int $prop1 {
        set(int $value) => $value * 2;
    }
    public $prop2 {
        #[A]
        get => $this->prop2;
        #[A(1), B]
        #[C]
        final set($value) {
            $this->prop2 = $value;
        }
    }
    public string $prop3 {
        set(#[SensitiveParameter] string|int $value) => (string)$value;
    }
    public $prop4 {
        #[A]
        final &get => $this->prop4;
    }
    public function __construct(public int $prop5 {
        #[A]
        set(int $value) => $value + 1;
    }) {
    }

})

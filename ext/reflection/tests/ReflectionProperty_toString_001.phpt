--TEST--
Using ReflectionProperty::__toString() with hooked properties (GH-17927)
--FILE--
<?php

interface IHookedDemo {
	public mixed $getOnly { get; }
	public mixed $setOnly { set; }
	public mixed $both { get; set; }
}
abstract class HookedDemo {
	abstract public mixed $getOnly { get; }
	abstract public mixed $setOnly { set; }
	abstract public mixed $both { get; set; }
}
class WithHooks {
	public mixed $getOnly {
		get => "always this string";
	}
	public mixed $setOnly {
		set => strtolower($value);
	}
	public mixed $both {
		get => $this->prop3;
		set => strtolower($value);
	}
}
class WithFinalHooks {
	public mixed $getOnly {
		final get => "always this string";
	}
	public mixed $setOnly {
		final set => strtolower($value);
	}
	public mixed $both {
		final get => $this->prop3;
		final set => strtolower($value);
	}
}
class WithMixedHooks {
	public mixed $getIsFinal {
		final get => "always this string";
		set => strtolower($value);
	}
	public mixed $setIsFinal {
		get => $this->setIsFinal;
		final set => strtolower($value);
	}
}
$classes = [
	IHookedDemo::class,
	HookedDemo::class,
	WithHooks::class,
	WithFinalHooks::class,
	WithMixedHooks::class,
];
foreach ( $classes as $clazz ) {
	echo "$clazz:\n";
	$ref = new ReflectionClass( $clazz );
	foreach ( $ref->getProperties() as $prop ) {
		echo $prop;
	}
	echo "\n";
}
?>
--EXPECT--
IHookedDemo:
Property [ abstract public virtual mixed $getOnly { get; } ]
Property [ abstract public virtual mixed $setOnly { set; } ]
Property [ abstract public virtual mixed $both { get; set; } ]

HookedDemo:
Property [ abstract public virtual mixed $getOnly { get; } ]
Property [ abstract public virtual mixed $setOnly { set; } ]
Property [ abstract public virtual mixed $both { get; set; } ]

WithHooks:
Property [ public virtual mixed $getOnly { get; } ]
Property [ public mixed $setOnly { set; } ]
Property [ public mixed $both { get; set; } ]

WithFinalHooks:
Property [ public virtual mixed $getOnly { final get; } ]
Property [ public mixed $setOnly { final set; } ]
Property [ public mixed $both { final get; final set; } ]

WithMixedHooks:
Property [ public mixed $getIsFinal { final get; set; } ]
Property [ public mixed $setIsFinal { get; final set; } ]

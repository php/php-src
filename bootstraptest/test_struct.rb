assert_equal 'Struct::Foo', %q{
  Struct.instance_eval { const_set(:Foo, nil) }
  Struct.new("Foo")
  Struct::Foo
}
